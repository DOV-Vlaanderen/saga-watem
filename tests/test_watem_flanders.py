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
    assert result.returncode == 0


def test_watem_prc(tmpdir):
    args = {
        "PARCEL_SHAPES": data_dir / "Adpe768698.shp",
        "LANDUSE": data_dir / "Landgebruik_def.sgrd",
        "PRC": tmpdir / "prc.sgrd",
        "WTZ": data_dir / "Wtze768698.shp",
        "WLAS": data_dir / "Wlase768698.shp",
        "SBN": data_dir / "Sbne768698.shp",
        "WBN": data_dir / "Wbne768698.shp",
        "WGA": data_dir / "Wgae768698.shp",
        "GBG": data_dir / "Gbge768698.shp",
        "GBA": data_dir / "Gbae768698.shp",
        "TRN": data_dir / "Trne768698.shp",
        "KNW": data_dir / "Knwe768698.shp",
    }
    run_saga("watem_flanders", "1", args)

    with rio.open(str(tmpdir / "prc.sdat")) as prc_file:
        prc = prc_file.read()

    assert prc.shape == (1, 735, 1134)


def test_watem_3x3(tmpdir):
    args = {
        "DEM": data_dir / "dhmvi_5m.sgrd",
        "PRC": data_dir / "prc.sgrd",
        "DEM_FILTER": tmpdir / "dem_filter.sgrd",
    }

    run_saga("watem_flanders", "2", args)

    with rio.open(str(tmpdir / "dem_filter.sdat")) as filtered_file:
        filtered = filtered_file.read()

    assert filtered.shape == (1, 735, 1134)


def test_watem_c_parcel(tmpdir):
    args = {
        "PARCEL_SHAPES": data_dir / "Adpe768698.shp",
        "LANDUSE": data_dir / "Landgebruik_def.sgrd",
        "C": tmpdir / "c.sgrd",
        "WTZ": data_dir / "Wtze768698.shp",
        "WLAS": data_dir / "Wlase768698.shp",
        "SBN": data_dir / "Sbne768698.shp",
        "WBN": data_dir / "Wbne768698.shp",
        "WGA": data_dir / "Wgae768698.shp",
        "GBG": data_dir / "Gbge768698.shp",
        "GBA": data_dir / "Gbae768698.shp",
        "TRN": data_dir / "Trne768698.shp",
        "KNW": data_dir / "Knwe768698.shp",
        "GRB_VHA": "1"
    }
    run_saga("watem_flanders", "4", args)

    with rio.open(str(tmpdir / "c.sdat")) as c_file:
        c = c_file.read()

    assert c.shape == (1, 735, 1134)

    args = {
        "PARCEL_SHAPES": data_dir / "Adpe768698.shp",
        "LANDUSE": data_dir / "Landgebruik_def.sgrd",
        "C": tmpdir / "c.sgrd",
        "GRB_VHA": "0"
    }
    run_saga("watem_flanders", "4", args)

    with rio.open(str(tmpdir / "c.sdat")) as c_file:
        c = c_file.read()

    assert c.shape == (1, 735, 1134)

@pytest.mark.xfail(reason="only supported in newer saga versions, eg bookworm")
def test_watem_c_parcel_tiff(tmpdir):
    args = {
        "PARCEL_SHAPES": data_dir / "Adpe768698.shp",
        "LANDUSE": data_dir / "Landgebruik_def.sgrd",
        "C": tmpdir / "c.sgrd",
        "WTZ": data_dir / "Wtze768698.shp",
        "WLAS": data_dir / "Wlase768698.shp",
        "SBN": data_dir / "Sbne768698.shp",
        "WBN": data_dir / "Wbne768698.shp",
        "WGA": data_dir / "Wgae768698.shp",
        "GBG": data_dir / "Gbge768698.shp",
        "GBA": data_dir / "Gbae768698.shp",
        "TRN": data_dir / "Trne768698.shp",
        "KNW": data_dir / "Knwe768698.shp",
        "GRB_VHA": "1"
    }
    run_saga("watem", "4", args)

    with rio.open(str(tmpdir / "c.sdat")) as c_file:
        c = c_file.read()

    assert c.shape == (1, 735, 1134)

    args = {
        "PARCEL_SHAPES": data_dir / "Adpe768698.shp",
        "LANDUSE": data_dir / "Landgebruik_def.sgrd",
        "C": tmpdir / "c.tif",
        "GRB_VHA": "0"
    }
    run_saga("watem_flanders", "4", args)

    with rio.open(str(tmpdir / "c.tif")) as c_file:
        c = c_file.read()

    assert c.shape == (1, 735, 1134)

    assert c[0, 200, 130] == 0.37
