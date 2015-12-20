#include "PCI1761.h"


bool PCI1761::hasOpened = false;
InstantDiCtrl * PCI1761::instantDiCtrl = NULL;//Create a 'InstantDiCtrl' for DI function.
InstantDoCtrl * PCI1761::instantDoCtrl = NULL;//Create a instantDoCtrl for DO function.