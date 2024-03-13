import os
import subprocess
from pathlib import Path

import pytest
import rasterio as rio
from test_topology import sagify


data_dir = Path("tests/data/langegracht")


def run_saga(library, module, args):
    cmd = ["saga_cmd", library, module] + sagify(args)
    print(cmd)
    os.environ["SAGA_TLB"] = "build/libs" # requires both watem and watem_flanders
    result = subprocess.run(cmd, capture_output=True)
    print(result)
    assert result.returncode == 0

def test_watem_complete(tmpdir):
    args = {
        "DEM": data_dir / "dhmvi_5m.sgrd",
        "PRC": data_dir / "prc.sgrd",
        "K": data_dir / "K.sgrd",
        "UPSLOPE_AREA": tmpdir / "upslope.sgrd",
        "LS": tmpdir / "ls.sgrd",
        "WATER_EROSION": tmpdir / "water_erosion.sgrd",

    }
    run_saga("watem_flanders", "5", args)

    with rio.open(str(tmpdir / "water_erosion.sdat")) as we_file:
        water_erosion = we_file.read()

    assert water_erosion.shape == (1, 735, 1134)

