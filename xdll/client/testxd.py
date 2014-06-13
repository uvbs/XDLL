
import ctypes
import msvcrt

dll = ctypes.WinDLL('TipsClient.dll')
dll.Startup()
print dir(dll)

print 'press any key to coninue...'
msvcrt.getch()
dll.Cleanup()
