#!/usr/bin/env python3
"""
Generate a single HTML dashboard from a folder of Valgrind XML reports.

Features
- Scans a folder recursively for *.xml files
- Parses each Valgrind XML report
- Builds one standalone HTML file
- Shows one collapsible section per report
- Inside each report, shows collapsible sections for each <error>
- Highlights PASS/FAIL status per report

Usage:
    python valgrind_folder_to_html.py /path/to/reports -o reports.html

Optional:
    python valgrind_folder_to_html.py /path/to/reports -o reports.html --recursive
"""

from __future__ import annotations

import argparse
import html
import sys
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from pathlib import Path


@dataclass
class Frame:
    fn: str = ""
    ip: str = ""
    directory: str = ""
    file_name: str = ""
    line: str = ""
    obj: str = ""


@dataclass
class ErrorEntry:
    kind: str = ""
    what: str = ""
    unique: str = ""
    leaked_bytes: str = ""
    leaked_blocks: str = ""
    auxwhat: list[str] = field(default_factory=list)
    frames: list[Frame] = field(default_factory=list)


@dataclass
class Report:
    file_path: Path
    file_name: str
    valid: bool
    parse_error: str = ""
    protocol_version: str = ""
    protocol_tool: str = ""
    command: str = ""
    pid: str = ""
    ppid: str = ""
    final_state: str = ""
    started: str = ""
    finished: str = ""
    all_heap_freed: str = ""
    bytes_in_use: str = "0"
    blocks_in_use: str = "0"
    allocs: str = "0"
    frees: str = "0"
    bytes_allocated: str = "0"
    errors_summary: str = "0"
    suppressed: str = "0"
    suppressed_contexts: str = "0"
    preamble: str = ""
    valgrind_exe: str = ""
    valgrind_flags: list[str] = field(default_factory=list)
    program_exe: str = ""
    errors: list[ErrorEntry] = field(default_factory=list)
    raw_xml: str = ""

    @property
    def passed(self) -> bool:
        return self.valid and len(self.errors) == 0 and self.errors_summary == "0"


def esc(value: object) -> str:
    return html.escape("" if value is None else str(value))


def find_text(node: ET.Element | None, path: str, default: str = "") -> str:
    if node is None:
        return default
    child = node.find(path)
    if child is None or child.text is None:
        return default
    return child.text.strip()


def find_all_text(node: ET.Element | None, path: str) -> list[str]:
    if node is None:
        return []
    out: list[str] = []
    for child in node.findall(path):
        if child.text and child.text.strip():
            out.append(child.text.strip())
    return out


def parse_frame(frame_node: ET.Element) -> Frame:
    return Frame(
        fn=find_text(frame_node, "fn"),
        ip=find_text(frame_node, "ip"),
        directory=find_text(frame_node, "dir"),
        file_name=find_text(frame_node, "file"),
        line=find_text(frame_node, "line"),
        obj=find_text(frame_node, "obj"),
    )


def parse_error(error_node: ET.Element) -> ErrorEntry:
    frames = [parse_frame(frame) for frame in error_node.findall("stack/frame")]
    return ErrorEntry(
        kind=find_text(error_node, "kind"),
        what=find_text(error_node, "what") or find_text(error_node, "xwhat/text"),
        unique=find_text(error_node, "unique"),
        leaked_bytes=find_text(error_node, "xwhat/leakedbytes"),
        leaked_blocks=find_text(error_node, "xwhat/leakedblocks"),
        auxwhat=find_all_text(error_node, "auxwhat"),
        frames=frames,
    )


def parse_report(xml_path: Path) -> Report:
    try:
        raw_xml = xml_path.read_text(encoding="utf-8")
    except OSError as exc:
        return Report(
            file_path=xml_path,
            file_name=xml_path.name,
            valid=False,
            parse_error=f"Could not read file: {exc}",
        )

    try:
        root = ET.fromstring(raw_xml)
    except ET.ParseError as exc:
        return Report(
            file_path=xml_path,
            file_name=xml_path.name,
            valid=False,
            parse_error=f"Invalid XML: {exc}",
            raw_xml=raw_xml,
        )

    if root.tag != "valgrindoutput":
        return Report(
            file_path=xml_path,
            file_name=xml_path.name,
            valid=False,
            parse_error=f"Unexpected root element '{root.tag}'",
            raw_xml=raw_xml,
        )

    preamble_lines = [line.text.strip() for line in root.findall("preamble/line") if line.text and line.text.strip()]
    statuses = root.findall("status")

    started = ""
    finished = ""
    final_state = ""
    for status in statuses:
        state = find_text(status, "state")
        timestamp = find_text(status, "time")
        if state == "RUNNING" and not started:
            started = timestamp
        elif state == "FINISHED" and not finished:
            finished = timestamp
        final_state = state or final_state

    command = ""
    if preamble_lines and preamble_lines[-1].startswith("Command: "):
        command = preamble_lines[-1].replace("Command: ", "", 1)
    if not command:
        command = find_text(root, "args/argv/exe")

    return Report(
        file_path=xml_path,
        file_name=xml_path.name,
        valid=True,
        protocol_version=find_text(root, "protocolversion"),
        protocol_tool=find_text(root, "protocoltool") or find_text(root, "tool"),
        command=command,
        pid=find_text(root, "pid"),
        ppid=find_text(root, "ppid"),
        final_state=final_state,
        started=started,
        finished=finished,
        all_heap_freed=find_text(root, "all_heap_blocks_freed"),
        bytes_in_use=find_text(root, "heap_summary/memory_in_use_at_exit/bytes", "0"),
        blocks_in_use=find_text(root, "heap_summary/memory_in_use_at_exit/blocks", "0"),
        allocs=find_text(root, "heap_summary/total_heap_usage/allocs", "0"),
        frees=find_text(root, "heap_summary/total_heap_usage/frees", "0"),
        bytes_allocated=find_text(root, "heap_summary/total_heap_usage/bytes_allocated", "0"),
        errors_summary=find_text(root, "error_summary/errors", "0"),
        suppressed=find_text(root, "error_summary/suppressed", "0"),
        suppressed_contexts=find_text(root, "error_summary/suppressed_contexts", "0"),
        preamble="\n".join(preamble_lines),
        valgrind_exe=find_text(root, "args/vargv/exe"),
        valgrind_flags=[arg.text.strip() for arg in root.findall("args/vargv/arg") if arg.text and arg.text.strip()],
        program_exe=find_text(root, "args/argv/exe"),
        errors=[parse_error(err) for err in root.findall("error")],
        raw_xml=raw_xml,
    )


def format_frame(frame: Frame) -> str:
    fn = frame.fn or "(unknown function)"
    location_parts: list[str] = []

    if frame.directory and frame.file_name:
        location_parts.append(f"{frame.directory}/{frame.file_name}{':' + frame.line if frame.line else ''}")
    elif frame.file_name:
        location_parts.append(f"{frame.file_name}{':' + frame.line if frame.line else ''}")
    elif frame.obj:
        location_parts.append(frame.obj)

    if frame.ip:
        location_parts.append(f"ip={frame.ip}")

    location = " · ".join(location_parts) if location_parts else "No source location available"

    return f"""
      <div class="stack-frame">
        <div class="frame-main">{esc(fn)}</div>
        <div class="frame-sub">{esc(location)}</div>
      </div>
    """


def format_error(error: ErrorEntry, idx: int, prefix: str) -> str:
    rows: list[str] = []

    if error.unique:
        rows.append(f"""
          <div class="row">
            <div class="label">Unique ID</div>
            <div class="value"><code>{esc(error.unique)}</code></div>
          </div>
        """)

    if error.leaked_bytes or error.leaked_blocks:
        rows.append(f"""
          <div class="row">
            <div class="label">Leak summary</div>
            <div class="value">{esc(error.leaked_bytes or '0')} bytes in {esc(error.leaked_blocks or '0')} block(s)</div>
          </div>
        """)

    if error.auxwhat:
        rows.append(f"""
          <div class="row">
            <div class="label">Additional context</div>
            <div class="value">{'<br>'.join(esc(x) for x in error.auxwhat)}</div>
          </div>
        """)

    stack_html = (
        '<div class="metric-sub" style="margin-top:12px;">No stack trace available.</div>'
        if not error.frames
        else f"""
        <div class="section inner-section">
          <h4>Stack Trace</h4>
          {''.join(format_frame(frame) for frame in error.frames)}
        </div>
        """
    )

    title = error.what or "No description available"
    kind = error.kind or f"Error {idx + 1}"

    return f"""
      <details class="error-item" id="{esc(prefix)}-error-{idx}">
        <summary>
          <div class="error-heading">
            <div class="error-kind">{esc(kind)}</div>
            <div class="error-what">{esc(title)}</div>
          </div>
          <div class="toggle">Expand details</div>
        </summary>
        <div class="error-body">
          <div class="list">
            {''.join(rows)}
          </div>
          {stack_html}
        </div>
      </details>
    """


def format_report(report: Report, idx: int) -> str:
    report_id = f"report-{idx}"
    status_class = "status-pill" if report.passed else "status-pill fail"

    if not report.valid:
        return f"""
        <details class="report-item">
          <summary>
            <div class="report-summary">
              <div>
                <div class="report-title">{esc(report.file_name)}</div>
                <div class="report-subtitle">Unreadable or invalid report</div>
              </div>
              <div class="{status_class}">INVALID</div>
            </div>
          </summary>
          <div class="report-body">
            <div class="card card-inner">
              <h3>Parse Error</h3>
              <pre>{esc(report.parse_error)}</pre>
            </div>
            <div class="card card-inner">
              <h3>Raw File Content</h3>
              <pre>{esc(report.raw_xml or '(unavailable)')}</pre>
            </div>
          </div>
        </details>
        """

    errors_html = (
        '<div class="ok">No <code>&lt;error&gt;</code> entries found.</div>'
        if not report.errors
        else "".join(format_error(err, i, report_id) for i, err in enumerate(report.errors))
    )

    flags_html = ", ".join(f"<code>{esc(flag)}</code>" for flag in report.valgrind_flags) or "-"

    return f"""
    <details class="report-item" {'open' if idx == 0 else ''}>
      <summary>
        <div class="report-summary">
          <div>
            <div class="report-title">{esc(report.file_name)}</div>
            <div class="report-subtitle">Command: <code>{esc(report.command or report.program_exe or '(unknown)')}</code></div>
          </div>
          <div class="{status_class}">{'PASS' if report.passed else f'FAIL · {len(report.errors)} error(s)'}</div>
        </div>
      </summary>

      <div class="report-body">
        <div class="grid">
          <div class="card">
            <div class="metric-label">Errors</div>
            <div class="metric-value">{esc(len(report.errors))}</div>
            <div class="metric-sub">Detected Memcheck issues</div>
          </div>
          <div class="card">
            <div class="metric-label">Heap in use at exit</div>
            <div class="metric-value">{esc(report.bytes_in_use)} bytes</div>
            <div class="metric-sub">{esc(report.blocks_in_use)} blocks remaining</div>
          </div>
          <div class="card">
            <div class="metric-label">Allocations / Frees</div>
            <div class="metric-value">{esc(report.allocs)} / {esc(report.frees)}</div>
            <div class="metric-sub">Heap activity across execution</div>
          </div>
          <div class="card">
            <div class="metric-label">Bytes allocated</div>
            <div class="metric-value">{esc(report.bytes_allocated)}</div>
            <div class="metric-sub">Total heap usage across execution</div>
          </div>
        </div>

        <div class="card card-inner">
          <h3>Execution Summary</h3>
          <div class="list">
            <div class="row">
              <div class="label">Tool / Protocol</div>
              <div class="value"><code>{esc(report.protocol_tool or '-')}</code> / <code>{esc(report.protocol_version or '-')}</code></div>
            </div>
            <div class="row">
              <div class="label">PID / PPID</div>
              <div class="value">{esc(report.pid or '-')} / {esc(report.ppid or '-')}</div>
            </div>
            <div class="row">
              <div class="label">Run state</div>
              <div class="value">{esc(report.final_state or '-')}</div>
            </div>
            <div class="row">
              <div class="label">Timestamps</div>
              <div class="value">Started at <code>{esc(report.started or '-')}</code>, finished at <code>{esc(report.finished or '-')}</code></div>
            </div>
            <div class="row">
              <div class="label">All heap blocks freed</div>
              <div class="value">{esc(report.all_heap_freed or '-')}</div>
            </div>
            <div class="row">
              <div class="label">Suppressed issues</div>
              <div class="value">{esc(report.suppressed)} suppressed, {esc(report.suppressed_contexts)} suppressed contexts</div>
            </div>
          </div>
        </div>

        <div class="card card-inner">
          <h3>Arguments</h3>
          <div class="list">
            <div class="row">
              <div class="label">Valgrind executable</div>
              <div class="value"><code>{esc(report.valgrind_exe or '-')}</code></div>
            </div>
            <div class="row">
              <div class="label">Valgrind flags</div>
              <div class="value">{flags_html}</div>
            </div>
            <div class="row">
              <div class="label">Program under test</div>
              <div class="value"><code>{esc(report.program_exe or '-')}</code></div>
            </div>
          </div>
        </div>

        <div class="card card-inner">
          <h3>Preamble</h3>
          <pre>{esc(report.preamble or 'No preamble available.')}</pre>
        </div>

        <div class="card card-inner">
          <h3>Error Details</h3>
          {errors_html}
        </div>
      </div>
    </details>
    """


def build_html(reports: list[Report], folder: Path) -> str:
    total = len(reports)
    passed = sum(1 for r in reports if r.passed)
    failed = sum(1 for r in reports if r.valid and not r.passed)
    invalid = sum(1 for r in reports if not r.valid)
    total_errors = sum(len(r.errors) for r in reports if r.valid)

    reports_html = (
        '<div class="card"><p>No XML files found.</p></div>'
        if not reports
        else "\n".join(format_report(report, i) for i, report in enumerate(reports))
    )

    return f"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>Valgrind Reports Dashboard</title>
  <style>
    :root {{
      --bg: #0b1020;
      --panel: #121a2b;
      --panel-2: #18233a;
      --text: #e8eefc;
      --muted: #a8b3cf;
      --good: #22c55e;
      --bad: #ef4444;
      --bad-bg: rgba(239, 68, 68, 0.08);
      --border: #2a3654;
    }}

    * {{ box-sizing: border-box; }}

    body {{
      margin: 0;
      font-family: Inter, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
      background: linear-gradient(180deg, var(--bg), #111827);
      color: var(--text);
      line-height: 1.5;
    }}

    .container {{
      max-width: 1200px;
      margin: 0 auto;
      padding: 32px 20px 48px;
    }}

    .header {{
      display: flex;
      flex-wrap: wrap;
      justify-content: space-between;
      gap: 16px;
      margin-bottom: 24px;
    }}

    .title h1 {{
      margin: 0 0 8px;
      font-size: 2rem;
      letter-spacing: -0.02em;
    }}

    .title p {{
      margin: 0;
      color: var(--muted);
    }}

    .grid {{
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
      gap: 16px;
      margin-bottom: 24px;
    }}

    .card {{
      background: linear-gradient(180deg, var(--panel), var(--panel-2));
      border: 1px solid var(--border);
      border-radius: 18px;
      padding: 18px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.18);
    }}

    .card-inner {{
      margin-top: 16px;
    }}

    .metric-label {{
      color: var(--muted);
      font-size: 0.92rem;
      margin-bottom: 6px;
    }}

    .metric-value {{
      font-size: 1.8rem;
      font-weight: 800;
      letter-spacing: -0.02em;
    }}

    .metric-sub {{
      margin-top: 6px;
      color: var(--muted);
      font-size: 0.9rem;
    }}

    .status-pill {{
      padding: 10px 14px;
      border-radius: 999px;
      border: 1px solid rgba(34, 197, 94, 0.35);
      background: rgba(34, 197, 94, 0.12);
      color: #bbf7d0;
      font-weight: 700;
      white-space: nowrap;
      display: inline-flex;
      align-items: center;
      height: fit-content;
    }}

    .status-pill.fail {{
      border-color: rgba(239, 68, 68, 0.35);
      background: rgba(239, 68, 68, 0.12);
      color: #fecaca;
    }}

    .list {{
      display: grid;
      gap: 10px;
    }}

    .row {{
      display: grid;
      grid-template-columns: 220px 1fr;
      gap: 12px;
      padding: 10px 0;
      border-bottom: 1px solid rgba(255,255,255,0.06);
    }}

    .row:last-child {{ border-bottom: 0; }}

    .label {{
      color: var(--muted);
      font-weight: 600;
    }}

    code, pre {{
      font-family: ui-monospace, SFMono-Regular, Menlo, Monaco, Consolas, monospace;
    }}

    pre {{
      margin: 0;
      white-space: pre-wrap;
      word-break: break-word;
      background: rgba(255,255,255,0.03);
      border: 1px solid rgba(255,255,255,0.06);
      border-radius: 14px;
      padding: 14px;
      color: #dbeafe;
      overflow-x: auto;
    }}

    .ok {{
      color: var(--good);
      font-weight: 700;
    }}

    details.report-item {{
      border: 1px solid var(--border);
      border-radius: 20px;
      background: linear-gradient(180deg, var(--panel), var(--panel-2));
      overflow: hidden;
      margin-bottom: 18px;
      box-shadow: 0 10px 30px rgba(0, 0, 0, 0.18);
    }}

    details.report-item > summary {{
      list-style: none;
      cursor: pointer;
      padding: 18px 20px;
      user-select: none;
    }}

    details.report-item > summary::-webkit-details-marker {{
      display: none;
    }}

    .report-summary {{
      display: flex;
      flex-wrap: wrap;
      justify-content: space-between;
      align-items: flex-start;
      gap: 14px;
    }}

    .report-title {{
      font-size: 1.15rem;
      font-weight: 800;
    }}

    .report-subtitle {{
      margin-top: 4px;
      color: var(--muted);
      font-size: 0.95rem;
    }}

    .report-body {{
      padding: 0 20px 20px;
    }}

    details.error-item {{
      border: 1px solid rgba(239, 68, 68, 0.24);
      border-radius: 16px;
      background: var(--bad-bg);
      overflow: hidden;
      margin-top: 12px;
    }}

    details.error-item summary {{
      list-style: none;
      cursor: pointer;
      padding: 16px 18px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      gap: 16px;
      user-select: none;
    }}

    details.error-item summary::-webkit-details-marker {{
      display: none;
    }}

    .error-heading {{
      display: flex;
      flex-direction: column;
      gap: 4px;
    }}

    .error-kind {{
      font-weight: 800;
      color: #fecaca;
    }}

    .error-what {{
      color: var(--text);
    }}

    .toggle {{
      color: var(--muted);
      font-size: 0.9rem;
      white-space: nowrap;
    }}

    .error-body {{
      padding: 0 18px 18px;
    }}

    .stack-frame {{
      padding: 10px 12px;
      border-radius: 12px;
      border: 1px solid rgba(255,255,255,0.06);
      background: rgba(255,255,255,0.03);
      margin-bottom: 10px;
    }}

    .stack-frame:last-child {{
      margin-bottom: 0;
    }}

    .frame-main {{
      font-weight: 700;
    }}

    .frame-sub {{
      color: var(--muted);
      margin-top: 4px;
      font-size: 0.92rem;
    }}

    .inner-section h4 {{
      margin: 0 0 10px;
      font-size: 1rem;
    }}

    .footer-note {{
      margin-top: 20px;
      color: var(--muted);
      font-size: 0.92rem;
    }}

    @media (max-width: 700px) {{
      .row {{
        grid-template-columns: 1fr;
      }}

      details.error-item summary,
      .report-summary {{
        flex-direction: column;
        align-items: flex-start;
      }}
    }}
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <div class="title">
        <h1>Valgrind Reports Dashboard</h1>
        <p>Folder: <code>{esc(folder)}</code></p>
      </div>
    </div>

    <div class="grid">
      <div class="card">
        <div class="metric-label">Reports found</div>
        <div class="metric-value">{total}</div>
        <div class="metric-sub">XML files processed</div>
      </div>
      <div class="card">
        <div class="metric-label">Passed</div>
        <div class="metric-value">{passed}</div>
        <div class="metric-sub">No reported Valgrind errors</div>
      </div>
      <div class="card">
        <div class="metric-label">Failed</div>
        <div class="metric-value">{failed}</div>
        <div class="metric-sub">Reports with one or more errors</div>
      </div>
      <div class="card">
        <div class="metric-label">Total errors</div>
        <div class="metric-value">{total_errors}</div>
        <div class="metric-sub">Across all valid reports</div>
      </div>
    </div>

    {reports_html}

    <p class="footer-note">Each report is collapsible, and each Valgrind error inside a report is also collapsible.</p>
  </div>
</body>
</html>
"""


def collect_xml_files(folder: Path, recursive: bool) -> list[Path]:
    pattern = "**/*.xml" if recursive else "*.xml"
    return sorted(path for path in folder.glob(pattern) if path.is_file())


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate one HTML dashboard from a folder of Valgrind XML reports."
    )
    parser.add_argument("folder", type=Path, help="Folder containing Valgrind XML files")
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=Path("valgrind_reports.html"),
        help="Output HTML file path",
    )
    parser.add_argument(
        "--recursive",
        action="store_true",
        help="Scan subfolders recursively for XML files",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    folder: Path = args.folder

    if not folder.exists():
        print(f"Folder does not exist: {folder}", file=sys.stderr)
        return 1
    if not folder.is_dir():
        print(f"Path is not a folder: {folder}", file=sys.stderr)
        return 1

    xml_files = collect_xml_files(folder, args.recursive)
    reports = [parse_report(path) for path in xml_files]
    html_output = build_html(reports, folder.resolve())

    try:
        args.output.write_text(html_output, encoding="utf-8")
    except OSError as exc:
        print(f"Could not write output file '{args.output}': {exc}", file=sys.stderr)
        return 1

    print(f"Generated HTML dashboard: {args.output}")
    print(f"Processed {len(xml_files)} XML file(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
