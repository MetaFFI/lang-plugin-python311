"""
Pytest fixtures for Python3 host compiler E2E tests.

Required environment variables:
- METAFFI_SOURCE_ROOT: Path to MetaFFI source root (for IDL files)
- METAFFI_HOME: Path to MetaFFI build output (for xllr.test plugin)
"""

import pytest
import subprocess
import os
import sys
import shutil
import platform


def get_xllr_test_plugin_path() -> str:
    """
    Get the path to the xllr.test shared library.

    Returns:
        str: Path to xllr.test.dll/.so/.dylib

    Raises:
        RuntimeError: If METAFFI_HOME not set
        FileNotFoundError: If plugin not found
    """
    metaffi_home = os.environ.get('METAFFI_HOME')
    if not metaffi_home:
        raise RuntimeError("METAFFI_HOME not set")

    # Determine library extension based on platform
    if platform.system() == 'Windows':
        lib_name = 'xllr.test.dll'
    elif platform.system() == 'Darwin':
        lib_name = 'libxllr.test.dylib'
    else:
        lib_name = 'libxllr.test.so'

    # The xllr.test plugin is built to $METAFFI_HOME/test/
    plugin_path = os.path.join(metaffi_home, 'test', lib_name)

    if not os.path.exists(plugin_path):
        raise FileNotFoundError(f"xllr.test plugin not found at: {plugin_path}")

    return plugin_path


@pytest.fixture(scope="module")
def generated_host_module():
    """
    Runs metaffi compiler to generate host code and returns the path to generated module.

    This fixture:
    1. Runs `metaffi -c --idl <xllr.test.idl.json> -h python3`
    2. Verifies the compiler exits with code 0
    3. Verifies host_MetaFFIHost.py is generated
    4. Adds the output directory to sys.path for importing

    Required environment variables:
    - METAFFI_SOURCE_ROOT: Path to MetaFFI source root (for IDL files)

    Yields:
        str: Path to the generated host_MetaFFIHost.py file
    """
    metaffi_source_root = os.environ.get('METAFFI_SOURCE_ROOT')
    if not metaffi_source_root:
        pytest.fail("METAFFI_SOURCE_ROOT not set")

    # Path to the xllr.test IDL file (in source tree)
    idl_path = os.path.join(
        metaffi_source_root, 'sdk', 'test_modules', 'guest_modules', 'test', 'xllr.test.idl.json'
    )

    if not os.path.exists(idl_path):
        pytest.fail(f"IDL file not found: {idl_path}")

    # Output directory (relative to this test file)
    test_dir = os.path.dirname(__file__)
    output_base_dir = os.path.join(test_dir, 'output')

    # The compiler creates ./test/host_MetaFFIHost.py relative to cwd
    # (based on the IDL module name 'test')
    test_output_dir = os.path.join(output_base_dir, 'test')

    # Clean and create output directory
    if os.path.exists(output_base_dir):
        shutil.rmtree(output_base_dir)
    os.makedirs(output_base_dir, exist_ok=True)

    # Run metaffi compiler
    result = subprocess.run(
        ['metaffi', '-c', '--idl', idl_path, '-h', 'python3'],
        capture_output=True,
        text=True,
        cwd=output_base_dir  # Run in output dir so ./test/ is created there
    )

    # Store compiler output for debugging
    if result.stdout:
        print(f"Compiler stdout:\n{result.stdout}")
    if result.stderr:
        print(f"Compiler stderr:\n{result.stderr}")

    assert result.returncode == 0, f"Compiler failed with code {result.returncode}:\n{result.stderr}"

    # Verify generated file exists
    generated_file = os.path.join(test_output_dir, 'host_MetaFFIHost.py')
    assert os.path.exists(generated_file), f"Generated file not found: {generated_file}"

    # Add output directory to path for importing
    if test_output_dir not in sys.path:
        sys.path.insert(0, test_output_dir)

    yield generated_file

    # Cleanup: remove from sys.path
    if test_output_dir in sys.path:
        sys.path.remove(test_output_dir)


@pytest.fixture(scope="module")
def bound_host_module(generated_host_module):
    """
    Returns the imported and bound host module, ready for testing.

    This fixture depends on generated_host_module and:
    1. Imports the generated host_MetaFFIHost module
    2. Calls bind_module_to_code() with the xllr.test plugin path

    Required environment variables:
    - METAFFI_HOME: Path to MetaFFI build output (for xllr.test plugin)

    Yields:
        module: The imported and bound host_MetaFFIHost module
    """
    # Import the generated module
    import host_MetaFFIHost as host

    # Bind to the xllr.test plugin using 'test' runtime
    plugin_path = get_xllr_test_plugin_path()
    host.bind_module_to_code(plugin_path, 'test')

    yield host
