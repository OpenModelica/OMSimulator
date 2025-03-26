## status: correct
## teardown_command:
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

from OMSimulator import CRef

x = CRef('foo', 'bar')
y = CRef('foo', 'bar', 'baz')
z = CRef(y)
print(x, y, z)

x.names[0] = 'x'
y.names[0] = 'y'
z.names[0] = 'z'
print(x, y, z)

## Result:
## foo/bar foo/bar/baz foo/bar/baz
## x/bar y/bar/baz z/bar/baz
## endResult
