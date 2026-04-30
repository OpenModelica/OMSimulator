# This file is part of OpenModelica.
#
# Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
# c/o Linköpings universitet, Department of Computer and Information Science,
# SE-58183 Linköping, Sweden.
#
# All rights reserved.
#
# THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
# THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
# ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
# RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
# VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
#
# The OpenModelica software and the OSMC (Open Source Modelica Consortium)
# Public License (OSMC-PL) are obtained from OSMC, either from the above
# address, from the URLs:
# http://www.openmodelica.org or
# https://github.com/OpenModelica/ or
# http://www.ida.liu.se/projects/OpenModelica,
# and in the OpenModelica distribution.
#
# GNU AGPL version 3 is obtained from:
# https://www.gnu.org/licenses/licenses.html#GPL
#
# This program is distributed WITHOUT ANY WARRANTY; without
# even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
# IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
#
# See the full OSMC Public License conditions for more details.

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

  def split(self):
    if self.is_root():
      return ("", self)
    else:
      return (self.first(), self.pop_first())

  def __lt__(self, other: object) -> bool:
    '''Compare two CRef objects based on their names.'''
    if not isinstance(other, CRef):
      raise TypeError("Expected a CRef object.")
    return self.names < other.names

  def __hash__(self) -> int:
    '''Return the hash value of the CRef.'''
    return hash(str(self))
