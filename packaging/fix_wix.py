import sys
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Element

ns = { "ns": "http://schemas.microsoft.com/wix/2006/wi" }
ET.register_namespace('',"http://schemas.microsoft.com/wix/2006/wi")

files_path = sys.argv[1]

# Load and parse the WiX file
# Our friend fix_wix.cmake passes us the path to it
files_tree = ET.parse(files_path)

def fix_files() -> bool:
    modified_files = False
    files_root = files_tree.getroot()
    # .// Finds the Shortcut on any level
    shortcut = files_root.find(".//ns:Shortcut[@Id='CM_SP_schetool.exe']", ns)
    # Add ShortcutProperty elements
    if (shortcut != None and sum(1 for prop in shortcut.iterfind("ns:ShortcutProperty", ns)) == 0):
        shortcut.append(Element("ShortcutProperty", {"Key": "System.AppUserModel.ID", "Value": "Kip.schetool"}))
        shortcut.append(Element("ShortcutProperty", {"Key": "System.AppUserModel.ToastActivatorCLSID", "Value": "{79A832A4-47BC-46CD-998A-73DCD7CAF255}"}))
        modified_files = True
    return modified_files

if (fix_files()):
    # Save the modified WiX files
    files_tree.write(files_path)
    print("Python fix_wix.py: Fixed files.wxs to include AUMID and Toast Notification GUID")
