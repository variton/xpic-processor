#!/usr/bin/env python3
import argparse
import html
import subprocess
from pathlib import Path
import xml.etree.ElementTree as ET


def run_tests(test_folder, output_folder):
    tests = sorted(
        f for f in test_folder.iterdir()
        if f.is_file()
        and f.name.startswith("test_")
        and f.stat().st_mode & 0o111
    )

    if not tests:
        raise RuntimeError(f"No executable test_* files found in {test_folder}")

    xml_files = []

    for test in tests:
        suffix = test.name.replace("test_", "", 1)
        output_xml = output_folder / f"doctest-results-{suffix}.xml"

        cmd = [
            str(test),
            "--reporters=junit",
            f"--out={output_xml}",
        ]

        print(f"Running: {' '.join(cmd)}")
        result = subprocess.run(cmd, check=False)

        if result.returncode != 0:
            print(f"[FAILED] {test.name}")
        else:
            print(f"[OK] Generated {output_xml}")

        if output_xml.exists():
            xml_files.append(output_xml)

    return xml_files


def to_int(value, default=0):
    try:
        return int(value)
    except (TypeError, ValueError):
        return default


def parse_xml(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    suites = []

    for suite in root.findall("testsuite"):
        testcases = []

        for case in suite.findall("testcase"):
            failed = case.find("failure") is not None
            error = case.find("error") is not None

            testcases.append({
                "name": case.get("name", ""),
                "classname": case.get("classname", ""),
                "time": case.get("time", "0"),
                "status": "ERROR" if error else "FAILED" if failed else "PASSED",
                "failed": failed or error,
            })

        suites.append({
            "name": suite.get("name", xml_file.stem),
            "file": xml_file.name,
            "tests": to_int(suite.get("tests"), len(testcases)),
            "failures": to_int(suite.get("failures")),
            "errors": to_int(suite.get("errors")),
            "timestamp": suite.get("timestamp", ""),
            "doctest_version": suite.get("doctest_version", ""),
            "testcases": testcases,
        })

    return suites


def generate_html(suites, output_html):
    total_tests = sum(s["tests"] for s in suites)
    total_failures = sum(s["failures"] for s in suites)
    total_errors = sum(s["errors"] for s in suites)
    total_passed = total_tests - total_failures - total_errors
    all_passed = total_failures == 0 and total_errors == 0

    suite_blocks = ""

    for suite in suites:
        suite_failed = suite["failures"] > 0 or suite["errors"] > 0
        suite_status = "FAILED" if suite_failed else "PASSED"
        suite_class = "failed" if suite_failed else "passed"

        rows = ""

        for case in suite["testcases"]:
            row_class = "failed-row" if case["failed"] else "passed-row"
            badge_class = "badge-failed" if case["failed"] else "badge-passed"

            rows += f"""
            <tr class="{row_class}">
                <td class="test-name">{html.escape(case["name"])}</td>
                <td>{html.escape(case["classname"])}</td>
                <td>{html.escape(case["time"])}s</td>
                <td>
                    <span class="badge {badge_class}">
                        {case["status"]}
                    </span>
                </td>
            </tr>
            """

        suite_blocks += f"""
        <details class="suite-card" {"open" if suite_failed else ""}>
            <summary>
                <div>
                    <span class="suite-name">{html.escape(suite["name"])}</span>
                    <span class="suite-file">{html.escape(suite["file"])}</span>
                </div>

                <div class="suite-meta">
                    <span>{suite["tests"]} tests</span>
                    <span>{suite["failures"]} failures</span>
                    <span>{suite["errors"]} errors</span>
                    <span class="badge badge-{suite_class}">
                        {suite_status}
                    </span>
                </div>
            </summary>

            <table>
                <thead>
                    <tr>
                        <th>Test Name</th>
                        <th>File</th>
                        <th>Time</th>
                        <th>Status</th>
                    </tr>
                </thead>

                <tbody>
                    {rows}
                </tbody>
            </table>

            <div class="metadata">
                <p><strong>Timestamp:</strong> {html.escape(suite["timestamp"])}</p>
                <p><strong>Doctest version:</strong> {html.escape(suite["doctest_version"])}</p>
            </div>
        </details>
        """

    page = f"""<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Doctest Report</title>

<style>
:root {{
    --bg: #0f0f12;
    --panel: #18181d;
    --panel-2: #202028;

    --purple: #7c3aed;
    --purple-light: #a78bfa;

    --green: #22c55e;
    --red: #ef4444;

    --text: #f5f5f7;
    --muted: #9ca3af;

    --border: #2a2a34;
}}

* {{
    box-sizing: border-box;
}}

body {{
    margin: 0;
    font-family: Inter, Arial, sans-serif;
    background:
        radial-gradient(circle at top left, #2a1245 0%, transparent 30%),
        radial-gradient(circle at top right, #1d1038 0%, transparent 25%),
        var(--bg);
    color: var(--text);
}}

.container {{
    max-width: 1300px;
    margin: 40px auto;
    padding: 24px;
}}

.header {{
    padding: 36px;
    border-radius: 22px;
    background:
        linear-gradient(
            135deg,
            rgba(124,58,237,0.35),
            rgba(0,0,0,0.9)
        );
    border: 1px solid rgba(167,139,250,0.2);
    box-shadow:
        0 10px 40px rgba(0,0,0,0.45),
        inset 0 1px 0 rgba(255,255,255,0.03);
}}

.header h1 {{
    margin: 0;
    font-size: 42px;
    font-weight: 800;
    letter-spacing: -1px;
}}

.header p {{
    margin-top: 10px;
    color: var(--muted);
    font-size: 16px;
}}

.summary {{
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
    gap: 18px;
    margin: 28px 0;
}}

.card,
.suite-card {{
    background: linear-gradient(
        180deg,
        rgba(255,255,255,0.02),
        rgba(255,255,255,0.01)
    );
    border: 1px solid var(--border);
    border-radius: 18px;
    box-shadow: 0 10px 30px rgba(0,0,0,0.35);
}}

.card {{
    padding: 26px;
}}

.card-title {{
    font-size: 12px;
    text-transform: uppercase;
    letter-spacing: 1px;
    color: var(--muted);
    margin-bottom: 10px;
}}

.card-value {{
    font-size: 38px;
    font-weight: 800;
}}

.passed {{
    color: var(--green);
}}

.failed {{
    color: var(--red);
}}

.status-banner {{
    padding: 20px 24px;
    border-radius: 18px;
    margin-bottom: 28px;
    font-size: 20px;
    font-weight: 700;
    background:
        linear-gradient(
            135deg,
            rgba(124,58,237,0.25),
            rgba(0,0,0,0.8)
        );
    border: 1px solid rgba(124,58,237,0.25);
    box-shadow: 0 8px 25px rgba(0,0,0,0.35);
}}

details {{
    margin-bottom: 20px;
    overflow: hidden;
}}

summary {{
    cursor: pointer;
    display: flex;
    justify-content: space-between;
    align-items: center;
    gap: 16px;
    list-style: none;
    padding: 24px;
}}

summary::-webkit-details-marker {{
    display: none;
}}

summary::before {{
    content: "▶";
    color: var(--purple-light);
    margin-right: 12px;
    font-size: 13px;
}}

details[open] summary::before {{
    content: "▼";
}}

.suite-name {{
    font-size: 22px;
    font-weight: 700;
}}

.suite-file {{
    display: block;
    margin-top: 6px;
    color: var(--muted);
    font-size: 13px;
}}

.suite-meta {{
    display: flex;
    align-items: center;
    gap: 14px;
    flex-wrap: wrap;
    color: var(--muted);
    font-size: 14px;
}}

table {{
    width: 100%;
    border-collapse: collapse;
    margin-top: 4px;
}}

thead {{
    background: rgba(124,58,237,0.12);
}}

th {{
    padding: 18px;
    text-align: left;
    text-transform: uppercase;
    font-size: 12px;
    letter-spacing: 1px;
    color: var(--purple-light);
    border-bottom: 1px solid var(--border);
}}

td {{
    padding: 18px;
    border-bottom: 1px solid rgba(255,255,255,0.04);
}}

tbody tr {{
    transition: background 0.2s ease;
}}

tbody tr:hover {{
    background: rgba(124,58,237,0.08);
}}

.passed-row {{
    background: rgba(34,197,94,0.04);
}}

.failed-row {{
    background: rgba(239,68,68,0.05);
}}

.test-name {{
    font-weight: 600;
}}

.badge {{
    display: inline-flex;
    align-items: center;
    justify-content: center;
    min-width: 90px;
    padding: 8px 12px;
    border-radius: 999px;
    font-size: 11px;
    font-weight: 800;
    letter-spacing: 1px;
    text-transform: uppercase;
}}

.badge-passed {{
    background: rgba(34,197,94,0.15);
    color: #4ade80;
    border: 1px solid rgba(34,197,94,0.2);
}}

.badge-failed {{
    background: rgba(239,68,68,0.15);
    color: #f87171;
    border: 1px solid rgba(239,68,68,0.2);
}}

.metadata {{
    padding: 22px 24px;
    color: var(--muted);
    font-size: 13px;
}}

.metadata strong {{
    color: var(--purple-light);
}}

@media (max-width: 900px) {{
    summary {{
        flex-direction: column;
        align-items: flex-start;
    }}

    .suite-meta {{
        margin-top: 10px;
    }}

    table {{
        display: block;
        overflow-x: auto;
    }}
}}
</style>
</head>

<body>
<div class="container">

    <div class="header">
        <h1>Doctest Report</h1>
        <p>Combined report from all test_* executables</p>
    </div>

    <div class="summary">
        <div class="card">
            <div class="card-title">Total Tests</div>
            <div class="card-value">{total_tests}</div>
        </div>

        <div class="card">
            <div class="card-title">Passed</div>
            <div class="card-value passed">{total_passed}</div>
        </div>

        <div class="card">
            <div class="card-title">Failures</div>
            <div class="card-value failed">{total_failures}</div>
        </div>

        <div class="card">
            <div class="card-title">Errors</div>
            <div class="card-value failed">{total_errors}</div>
        </div>
    </div>

    <div class="status-banner">
        Overall status: {"PASSED" if all_passed else "FAILED"}
    </div>

    {suite_blocks}

</div>
</body>
</html>
"""

    output_html.write_text(page, encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(
        description="Run doctest C++ test executables and generate a combined HTML report."
    )

    parser.add_argument(
        "--input",
        required=True,
        help="Folder containing executable test_* files"
    )

    parser.add_argument(
        "--output",
        required=True,
        help="Folder where XML and HTML reports are generated"
    )

    parser.add_argument(
        "--html-name",
        default="doctest-report.html",
        help="Output HTML file name"
    )

    args = parser.parse_args()

    test_folder = Path(args.input).resolve()
    output_folder = Path(args.output).resolve()

    if not test_folder.exists():
        raise RuntimeError(f"Input folder does not exist: {test_folder}")

    output_folder.mkdir(parents=True, exist_ok=True)

    xml_files = run_tests(test_folder, output_folder)

    suites = []
    for xml_file in xml_files:
        suites.extend(parse_xml(xml_file))

    output_html = output_folder / args.html_name
    generate_html(suites, output_html)

    print(f"\nGenerated HTML report:")
    print(output_html)


if __name__ == "__main__":
    main()
