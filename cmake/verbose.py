from sys import argv

if '--verbose' in argv:
    argv.remove('--verbose')
    def verbose(*args, **kwargs):
        print(*args, file=stderr, **kwargs)
else:
    def verbose(*args, **kwargs):
        pass

