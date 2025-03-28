class CRef:
  def __init__(self, *names: str):
    '''Initialize a component reference with a list of hierarchical names.'''
    self.names = []
    for name in names:
      if isinstance(name, CRef):
        self.names += name.names
      else:
        self.names.append(str(name))

  def __str__(self) -> str:
    '''Return the string representation of the component reference.'''
    return '/'.join(self.names)

  def __repr__(self) -> str:
    '''Return a developer-friendly string representation of the CRef.'''
    return f"CRef{self.names}"

  def __eq__(self, other: object) -> bool:
    '''Check equality between two CRef objects.'''
    return isinstance(other, CRef) and self.names == other.names

  def add(self, name: str) -> 'CRef':
    '''Return a new CRef with an additional name at the end.'''
    return CRef(*self.names, name)

  def __truediv__(self, name: str) -> 'CRef':
    '''Return a new CRef with an additional name at the end.'''
    return CRef(*self.names, name)

  def parent(self) -> 'CRef | None':
    '''Return a new CRef representing the parent of this reference.'''
    if len(self.names) > 1:
      return CRef(*self.names[:-1])
    return None  # No parent exists

  def depth(self) -> int:
    '''Return the depth of the hierarchy.'''
    return len(self.names)

  def is_root(self) -> bool:
    '''Check if this is the root component (single name).'''
    return len(self.names) == 1

  def starts_with(self, other: 'CRef') -> bool:
    '''Check if this reference starts with another reference.'''
    if not isinstance(other, CRef):
      raise TypeError("Expected a CRef object.")
    return self.names[:len(other.names)] == other.names

  def to_list(self) -> list[str]:
    '''Return the list of names as a hierarchy.'''
    return self.names

  def pop_first(self, first: str = None) -> 'CRef | None':
    '''Return a new CRef without the first name.'''
    if len(self.names) == 0:
      raise ValueError("Cannot pop first name from an empty CRef.")

    if first is not None and self.names[0] != first:
      raise ValueError(f"Expected first name to be '{first}', but got '{self.names[0]}'")

    if len(self.names) == 1:
      return None
    return CRef(*self.names[1:])

  def first(self) -> 'CRef':
    '''Return the first name as a CRef.'''
    if len(self.names) == 0:
      raise ValueError("Empty CRef.")
    return CRef(self.names[0])

  def last(self) -> 'CRef':
    '''Return the last name as a CRef.'''
    if len(self.names) == 0:
      raise ValueError("Empty CRef.")
    return CRef(self.names[-1])

  def __lt__(self, other: object) -> bool:
    '''Compare two CRef objects based on their names.'''
    if not isinstance(other, CRef):
      raise TypeError("Expected a CRef object.")
    return self.names < other.names

  def __hash__(self) -> int:
    '''Return the hash value of the CRef.'''
    return hash(str(self))
