class CRef:
  def __init__(self, *names):
    '''Initialize a component reference with a list of hierarchical names.'''
    self.names = []
    for name in names:
      if isinstance(name, CRef):
        self.names += name.names
      else:
        self.names.append(str(name))

  def __str__(self):
    '''Return the string representation of the component reference.'''
    return '/'.join(self.names)

  def __repr__(self):
    return f"CRef{self.names}"

  def __eq__(self, other):
    return isinstance(other, CRef) and self.names == other.names

  def add(self, name):
    '''Return a new CRef with an additional name at the end.'''
    return CRef(*self.names, name)

  def __truediv__(self, name):
    '''Return a new CRef with an additional name at the end.'''
    return CRef(*self.names, name)

  def parent(self):
    '''Return a new CRef representing the parent of this reference.'''
    if len(self.names) > 1:
      return CRef(*self.names[:-1])
    return None  # No parent exists

  def depth(self):
    '''Return the depth of the hierarchy.'''
    return len(self.names)

  def is_root(self):
    '''Check if this is the root component (single name).'''
    return len(self.names) == 1

  def starts_with(self, other):
    '''Check if this reference starts with another reference.'''
    if not isinstance(other, CRef):
      raise TypeError("Expected a CRef object.")
    return self.names[:len(other.names)] == other.names

  def to_list(self):
    '''Return the list of names as a hierarchy.'''
    return self.names

  def pop_first(self, first=None):
    '''Return a new CRef without the first name.'''
    if len(self.names) == 0:
      raise ValueError("Cannot pop first name from an empty CRef.")

    if first is not None:
      if self.names[0] != first:
        raise ValueError(f"Expected first name to be '{first}', but got '{self.names[0]}'")

    if len(self.names) == 1:
      return None
    else:
      return CRef(*self.names[1:])

  def __lt__(self, other):
    '''Compare two CRef objects based on their names'''
    if not isinstance(other, CRef):
      raise TypeError('Expected a CRef object.')
    return self.names < other.names
