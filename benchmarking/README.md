# Benchmark reports

Run the command in `benchmark.json` and generate timing percentiles and an HTML report:

```bash
./percentiles.py --conf=benchmark.json --html=report.html
```

The configuration uses named settings inside a `command` object:

```json
{
  "command": {
    "runner": "./run.py",
    "xtimes": 50,
    "binary_path": "../bin/mt-mean-ssim",
    "original_image_path": "../rc/ur.jpg",
    "wartermarked_image_path": "../rc/watermarked_ur.jpg"
  }
}
```

`xtimes` is a positive integer. The three image/binary paths are required.
`watermarked_image_path` is also accepted as the corrected spelling of
`wartermarked_image_path`; use only one spelling. Optional settings are `runner`
(default `./run.py`), `json` (default `res.json`), and `log` (append output to a file).

Relative paths resolve from the configuration file's directory. Paths containing
spaces need no extra shell quoting. Path settings support `$NAME` and `${NAME}`
from the current environment. Unset variables produce an error before execution;
expansion is not recursive. JSON comments, other shell expansions, and pipelines
are not supported. JSON capture is added automatically.
The HTML destination is relative to your current working directory.

For example, the path settings can use `"$LAB/bin/mt-mean-ssim"`,
`"${LAB}/rc/ur.jpg"`, and `"${LAB}/rc/watermarked_ur.jpg"`. Supply `LAB` when running:

```bash
LAB=/path/to/xpic-processor ./percentiles.py --conf=benchmark.json --html=report.html
```

If the binary needs the project's shared libraries, run from this directory with:

```bash
LD_LIBRARY_PATH="../lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" ./percentiles.py --conf=benchmark.json --html=report.html
```

To generate a report from existing results without running the benchmark:

```bash
./percentiles.py --input=res.json --html=report.html
```
