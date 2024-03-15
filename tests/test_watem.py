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
    os.environ["SAGA_TLB"] = "build/watem/watem"
    result = subprocess.run(cmd, capture_output=True)
    assert result.returncode == 0


def test_watem_uparea(tmpdir):
    args = {
        "DEM": data_dir / "dhmvi_5m.sgrd",
        "PRC": data_dir / "prc.sgrd",
        "PIT": tmpdir / "pit.sgrd",
        "UPSLOPE_AREA": tmpdir / "upslope_area.sgrd",
        "PITDATA": tmpdir / "pitdata.txt",
    }
    run_saga("watem", "1", args)


def test_watem_ls(tmpdir):
    args = {
        "DEM": data_dir / "dhmvi_5m.sgrd",
        "PRC": data_dir / "prc.sgrd",
        "PIT": tmpdir / "pit.sgrd",
        "UPSLOPE_AREA": tmpdir / "upslope_area.sgrd",
        "PITDATA": tmpdir / "pitdata.txt",
    }
    # tcreate upslope area first
    run_saga("watem", "1", args)

    args = {"DEM": data_dir / "dhmvi_5m.sgrd",
            "UPSLOPE_AREA": tmpdir / "upslope_area.sgrd",
            "USEPRC": "1",
            "PRC": data_dir / "prc.sgrd",
            "LS": tmpdir / "ls.sgrd",
            }

    # test running with optional slope,l,s output
    run_saga("watem", "2", args)

    # test with optional slope
    args = {**args,"SLOPE": tmpdir / "slope.sgrd", "L": tmpdir/"l.sgrd", "S": tmpdir/"s.sgrd"}
    run_saga("watem", "2", args)

    with rio.open(str(tmpdir / "slope.sdat")) as slope_file:
        slope = slope_file.read()
    with rio.open(str(tmpdir / "ls.sdat")) as ls_file:
        ls = ls_file.read()
    assert slope.shape == (1, 735, 1134)
    assert ls.shape == (1, 735, 1134)

    # check that the grid contains anything
    assert pytest.approx(slope[0,100, 200]) == 1e-6
    assert pytest.approx(ls[0,100, 200]) == 0.030010605
