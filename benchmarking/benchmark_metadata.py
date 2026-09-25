"""Collect machine details at benchmark start without external dependencies."""

from datetime import datetime, timezone
import os
from pathlib import Path
import platform


def collect_metadata(command):
    model = platform.processor() or None
    physical_cores = None
    try:
        records = []
        for block in Path('/proc/cpuinfo').read_text().strip().split('\n\n'):
            records.append(dict(line.split(':', 1) for line in block.splitlines() if ':' in line))
        records = [{key.strip(): value.strip() for key, value in record.items()} for record in records]
        model = next((r['model name'] for r in records if r.get('model name')), model)
        cores = {(r['physical id'], r['core id']) for r in records
                 if 'physical id' in r and 'core id' in r}
        if cores and all('physical id' in r and 'core id' in r for r in records):
            physical_cores = len(cores)
    except OSError:
        pass
    try:
        available_cpus = len(os.sched_getaffinity(0))
    except (AttributeError, OSError):
        available_cpus = None
    return {
        'started_at_utc': datetime.now(timezone.utc).isoformat(),
        'command': list(command),
        'cpu_model': model,
        'physical_cores': physical_cores,
        'logical_cpus': os.cpu_count(),
        'affinity_logical_cpus': available_cpus,
        'os': f'{platform.system()} {platform.release()}',
        'architecture': platform.machine() or None,
    }
