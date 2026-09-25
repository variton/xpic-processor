"""Render a standalone, offline benchmark report."""

from datetime import datetime, timezone
from html import escape
import math
import statistics


def render_report(results, percentiles, source, command=None):
    times = [item['time_elapsed_ms'] for item in results]
    count = len(times)
    mean = statistics.mean(times)
    deviation = statistics.pstdev(times)
    low, high = min(times), max(times)
    scores = [item.get('mean_ssim') for item in results]
    valid_scores = [s for s in scores if isinstance(s, (int, float))
                    and not isinstance(s, bool) and math.isfinite(s)]
    generated = datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M:%S UTC')

    def card(label, value, note=''):
        return f'<article class="card"><span>{label}</span><strong>{value}</strong><small>{note}</small></article>'

    cards = ''.join(card(name.upper(), f'{value:.3f}<em>ms</em>',
                         f'{name[1:]}th percentile') for name, value in percentiles.items())
    stats = ''.join(card(label, value, note) for label, value, note in [
        ('SAMPLES', str(count), 'Recorded timing entries'),
        ('MEAN', f'{mean:.3f}<em>ms</em>', 'Arithmetic average'),
        ('MIN / MAX', f'{low:.3f} / {high:.3f}', 'Milliseconds'),
        ('STD DEVIATION', f'{deviation:.3f}<em>ms</em>', 'Population standard deviation'),
        ('VARIATION', f'{deviation / mean * 100:.2f}%' if mean else 'N/A', 'Standard deviation / mean'),
        ('TOTAL TIMED', f'{sum(times) / 1000:.3f}<em>s</em>', 'Sum of reported timings; not wall time'),
    ])
    # Fixed SVG coordinates scale responsively without external chart libraries.
    width, height = 900, 260
    bottom, top = 220, 25
    y_min = max(0, low - max((high - low) * .2, 1))
    y_max = high + max((high - low) * .2, 1)
    def x(i):
        return 65 + i / max(count - 1, 1) * 805
    def y(value):
        return bottom - (value - y_min) / (y_max - y_min) * (bottom - top)
    grid = ''.join(f'<line class="grid" x1="65" x2="870" y1="{y(v)}" y2="{y(v)}"/>'
                   f'<text x="55" y="{y(v) + 4}" text-anchor="end">{v:.1f}</text>'
                   for v in [y_min + (y_max - y_min) * i / 4 for i in range(5)])
    points = ' '.join(f'{x(i):.2f},{y(v):.2f}' for i, v in enumerate(times))
    dots = ''.join(f'<circle cx="{x(i)}" cy="{y(v)}" r="4"><title>Sample {i+1}: {v:.3f} ms</title></circle>'
                   for i, v in enumerate(times))
    chart = (f'<svg viewBox="0 0 {width} {height}" role="img" aria-label="Execution time by sample">'
             f'{grid}<line class="threshold" x1="65" x2="870" y1="{y(percentiles["p95"])}" y2="{y(percentiles["p95"])}"/>'
             f'<polyline points="{points}"/>{dots}'
             f'<text x="65" y="248">Sample 1</text><text x="870" y="248" text-anchor="end">Sample {count}</text></svg>')
    bins = min(10, max(1, math.ceil(math.sqrt(count)))) if high != low else 1
    step = (high - low) / bins if high != low else 1
    frequencies = [0] * bins
    for value in times:
        frequencies[min(bins - 1, int((value - low) / step))] += 1
    histogram = ''
    for i, frequency in enumerate(frequencies):
        start = low + i * step
        end = start + step if high != low else low
        bar_height = frequency / max(frequencies) * 170
        bx = 65 + i * 805 / bins
        histogram += (f'<rect x="{bx + 4}" y="{220 - bar_height}" width="{805 / bins - 8}" height="{bar_height}">'
                      f'<title>{start:.3f}–{end:.3f} ms: {frequency} samples</title></rect>'
                      f'<text x="{bx + 805 / bins / 2}" y="{210 - bar_height}" text-anchor="middle">{frequency}</text>')
    histogram = (f'<svg viewBox="0 0 900 260" role="img" aria-label="Timing frequency distribution">{histogram}'
                 f'<text x="65" y="248">{low:.3f} ms</text><text x="870" y="248" text-anchor="end">{high:.3f} ms</text></svg>')
    rows = ''
    for i, (value, score) in enumerate(zip(times, scores), start=1):
        score_text = f'{score:.6f}' if isinstance(score, (int, float)) and not isinstance(score, bool) and math.isfinite(score) else 'N/A'
        rows += f'<tr><td>{i:03d}</td><td>{value:.3f}</td><td>{score_text}</td><td>{"Above p95" if value > percentiles["p95"] else "—"}</td></tr>'
    quality = (f'{statistics.mean(valid_scores):.6f} mean · {min(valid_scores):.6f} min · '
               f'{max(valid_scores):.6f} max · {len(valid_scores)}/{count} samples') if valid_scores else 'No SSIM values recorded'
    provenance = ('Executed by this script' if command else 'User-provided benchmark command; execution metadata is not stored in the input JSON')
    command = command or './run.py -xtimes=50 --json=res.json ../bin/mt-mean-ssim ../rc/ur.jpg ../rc/watermarked_ur.jpg'
    return f'''<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>SSIM Benchmark · Matrix Report</title>
<style>
:root{{color-scheme:dark;--green:#68ff9b;--muted:#95bda3;--line:#224a32;--panel:#08180f}}
*{{box-sizing:border-box}}body{{margin:0;background:#030906;background-image:linear-gradient(#14311d30 1px,transparent 1px),linear-gradient(90deg,#14311d30 1px,transparent 1px);background-size:40px 40px;color:#e5f9eb;font:15px/1.6 ui-monospace,SFMono-Regular,Consolas,monospace}}
main{{max-width:1240px;margin:auto;padding:48px 28px}}header{{border-bottom:1px solid var(--line);padding-bottom:28px;margin-bottom:28px}}.eyebrow{{color:var(--green);letter-spacing:3px;font-size:12px}}h1{{font-size:clamp(30px,5vw,54px);line-height:1.15;margin:15px 0;text-shadow:0 0 30px #68ff9b35}}h2{{font-size:17px;margin:0 0 18px}}p,small,.muted{{color:var(--muted)}}.cards{{display:grid;grid-template-columns:repeat(4,1fr);gap:14px;margin-bottom:18px}}.stats{{grid-template-columns:repeat(3,1fr)}}.card,section{{background:var(--panel);border:1px solid var(--line);border-radius:8px;padding:22px}}.card span{{display:block;font-size:12px;letter-spacing:2px;color:var(--muted)}}.card strong{{display:block;font-size:28px;font-weight:500;color:var(--green);margin:10px 0}}.stats strong{{font-size:23px}}em{{font-size:14px;font-style:normal;padding-left:8px;color:var(--muted)}}small{{font-size:11px}}section{{margin:18px 0}}.charts{{display:grid;grid-template-columns:1fr 1fr;gap:18px}}.charts section{{min-width:0}}svg{{width:100%;height:auto;overflow:visible}}svg text{{fill:var(--muted);font-size:12px}}.grid{{stroke:var(--line)}}polyline{{stroke:var(--green);fill:none;stroke-width:2}}circle,rect{{fill:var(--green)}}circle:hover{{r:7;fill:white}}.threshold{{stroke:#e4cf7a;stroke-dasharray:7 5}}code{{display:block;white-space:pre-wrap;overflow-wrap:anywhere;color:var(--green);background:#030906;padding:16px;border-radius:5px}}dl{{display:grid;grid-template-columns:160px 1fr;gap:10px}}dt{{color:var(--muted)}}dd{{margin:0;overflow-wrap:anywhere}}.table-wrap{{max-height:480px;overflow:auto}}table{{width:100%;border-collapse:collapse;text-align:right}}th,td{{padding:10px 14px;border-bottom:1px solid var(--line)}}th{{position:sticky;top:0;background:#102719;color:var(--green)}}th:first-child,td:first-child{{text-align:left}}tr:hover{{background:#14311d}}button{{background:var(--green);border:0;border-radius:4px;padding:9px 16px;font:inherit;cursor:pointer;color:#031008}}footer{{font-size:12px;color:var(--muted);margin-top:28px}}@media(max-width:800px){{.cards,.stats,.charts{{grid-template-columns:repeat(2,1fr)}}.charts{{grid-template-columns:1fr}}main{{padding:24px 14px}}dl{{grid-template-columns:1fr}}}}@media(max-width:450px){{.cards,.stats{{grid-template-columns:1fr}}}}@media print{{body{{background:white;color:black}}main{{padding:0}}.card,section{{break-inside:avoid;background:white}}.card strong,h1,h2,code,th,svg text{{color:#145b2c;fill:#145b2c}}.table-wrap{{max-height:none}}button{{display:none}}}}
</style></head><body><main>
<header><div class="eyebrow">BENCHMARK TELEMETRY // MT-MEAN-SSIM</div><h1>Execution time. Decoded.</h1><p>Timing percentiles and image similarity · {count} recorded samples</p><button onclick="window.print()">Print / Save PDF</button></header>
<div class="cards">{cards}</div><div class="cards stats">{stats}</div>
<div class="charts"><section><h2>01 // Sample timeline</h2>{chart}<small>Execution order · milliseconds · dashed amber line = p95 · hover for values</small></section><section><h2>02 // Timing distribution</h2>{histogram}<small>Equal-width bins · counts above bars · hover for ranges</small></section></div>
<section><h2>03 // Image similarity</h2><p>{quality}</p><small>Values are reported by the benchmark. A rounded value of 1.0 alone does not establish that input files are identical.</small></section>
<section><h2>04 // Benchmark context</h2><dl><dt>Results source</dt><dd>{escape(str(source))}</dd><dt>Report generated</dt><dd>{generated}</dd><dt>Command provenance</dt><dd>{provenance}</dd></dl><code>{escape(command)}</code><p>Sequential command invocations. The supplied command requests 50 runs and compares <b>ur.jpg</b> with <b>watermarked_ur.jpg</b>.</p><small>Hardware, build flags, thread count, environment, and benchmark execution timestamp are not recorded in the results file.</small></section>
<section><h2>05 // Methodology</h2><p>Percentiles use sorted execution times and linear interpolation at index (N − 1) × p / 100. P50 is the median; p95 describes the upper timing tail. All recorded timing samples are included; no warm-up removal or outlier filtering is applied.</p><p>Timing is the binary’s reported <code style="display:inline;padding:2px">time_elapsed_ms</code>, not process startup or overall wall-clock duration. Standard deviation is calculated over the full recorded population. With {count} samples, tail percentiles are descriptive estimates; no confidence interval is calculated.</p></section>
<section><h2>06 // All samples</h2><div class="table-wrap"><table><thead><tr><th scope="col">Sample</th><th scope="col">Time (ms)</th><th scope="col">Mean SSIM</th><th scope="col">Timing note</th></tr></thead><tbody>{rows}</tbody></table></div></section>
<footer>SELF-CONTAINED REPORT // No external fonts, scripts, or network requests.</footer>
</main></body></html>'''
