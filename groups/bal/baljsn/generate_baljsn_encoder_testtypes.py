#!/opt/bb/bin/python3.7
"""This module defines a program that generates the 'baljsn_encoder_testtypes'
component and replace all uses of 'bdes' with 'bsls' within its files.
"""

from asyncio import create_subprocess_exec as aio_create_subprocess_exec
from asyncio import run as aio_run
from asyncio import subprocess as aio_subprocess

from mmap import mmap as mm_mmap

from re import compile as re_compile
from re import finditer as re_finditer

from sys import exit as sys_exit
from sys import version_info as sys_version_info

from typing import AsyncGenerator as ty_AsyncGenerator
from typing import TypeVar as ty_TypeVar
from typing import Union as ty_Union
from typing import cast as ty_cast

T = ty_TypeVar('T')

def not_none_cast(x: ty_Union[T, None]) -> T:
    """Return the specified `x` cast to the specified `T` type.

    Args:
        x (typing.Union[T, None]): the value to return

    Returns:
        T: the specified `x` cast to `T`
    """
    return ty_cast(T, x)

async def generate_components() -> ty_AsyncGenerator[str, None]:
    """Generate the `baljsn_encoder_testtypes` components.

    Spawn a subprocess that generates the C++ code for the
    `baljsn_encoder_testtypes.xsd` schema.  Return an async generator `G` that
    yields each line of output from the subprocess as it is received and that
    returns upon termination of the process.  Note that this function does not
    have a dependency on the shell of the user, but does depend on the user's
    executable search path, since it directly executes `bas_codegen.pl`.

    Returns:
        typing.AsyncGenerator[str, None]: `G`
    """
    process = await aio_create_subprocess_exec(
            'bas_codegen.pl',
            '--mode',
            'msg',
            '--noAggregateConversion',
            '--noExternalization',
            '--msgComponent=encoder_testtypes',
            '--package=baljsn',
            'baljsn_encoder_testtypes.xsd',
            stdout=aio_subprocess.PIPE,
            stderr=aio_subprocess.STDOUT)
    stdout = not_none_cast(process.stdout)
    while not stdout.at_eof():
        line: bytes = await stdout.readline()
        if not stdout.at_eof or len(line) != 0:
            yield line.decode()
    await process.communicate()

def rewrite_bdes_ident_to_bsls(file: str) -> None:
    """Replace all occurrences of "bdes_ident" with "bsls_ident" in the
    specified `file`.

    Args:
        file (str): an absolute or relative path to a file

    Returns:
        None
    """
    with open(file, "r+b") as f, mm_mmap(f.fileno(), 0) as filemap:
        regex = b'(?P<LOWER_CASE>bdes_ident)|(?P<UPPER_CASE>BDES_IDENT)'
        compiled_regex = re_compile(regex)
        # mmap objects satisfy the bytearray interface
        filemap_bytearray = ty_cast(bytearray, filemap)
        for match in re_finditer(compiled_regex, filemap_bytearray):
            group = match.lastgroup
            if group == 'LOWER_CASE':
                filemap[match.start():match.end()] = b'bsls_ident'
            else:
                assert group == 'UPPER_CASE'
                filemap[match.start():match.end()] = b'BSLS_IDENT'
        filemap.flush()

async def main() -> None:
    """Asynchronously generate the 'baljsn_encdoer_testypes' components and
    replace all occurrences of "bdes_ident" with "bsls_ident" within them.

    Return:
        None
    """
    print("Generating files with bas_codegen.pl")
    lines = generate_components()
    async for line in lines:
        print(line.strip('\n'))
    print("Replacing 'bdes_ident' with 'bsls_ident' in " +
            "baljsn_encoder_testtypes.h")
    rewrite_bdes_ident_to_bsls('./baljsn_encoder_testtypes.h')
    print("Replacing 'bdes_ident' with 'bsls_ident' in " +
            "baljsn_encoder_testtypes.cpp")
    rewrite_bdes_ident_to_bsls('./baljsn_encoder_testtypes.cpp')

if __name__ == '__main__':
    if not sys_version_info.major == 3 and sys_version_info.minor >= 6:
        print("This program requires Python 3.6 or higher")
        sys_exit(1)
    aio_run(main())

# ----------------------------------------------------------------------------
# Copyright 2020 Bloomberg Finance L.P.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ----------------------------- END-OF-FILE ----------------------------------
