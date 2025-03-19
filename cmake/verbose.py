
from sys import argv, stderr

if '--verbose' in argv:
    argv.remove('--verbose')

    def verbose(*args, **kwargs):
        print(*args, file=stderr, **kwargs)
else:
    def verbose(*_args, **_kwargs):
        pass

