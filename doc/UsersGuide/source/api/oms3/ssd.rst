SSD
---

The **SSV** class provides a high-level interface for importing,
and managing SSD (System Structure Description) models which are
unpacked from SSP files. We can use the SSD class to load an SSD file,
inspect its structure, and manipulate its elements programmatically.

.. code-block:: python

  from OMSimulator import SSD, Settings
  Settings.suppressPath = True
  ssd = SSD.importFromFile('../resources/LOC/SystemStructure.ssd')
  ssd.list()
