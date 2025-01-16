import sys
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Element

ns = { "ns": "http://schemas.microsoft.com/wix/2006/wi" }
ET.register_namespace('',"http://schemas.microsoft.com/wix/2006/wi")

files_path = sys.argv[1]
features_path = sys.argv[2]

# Load and parse the WiX file
# Our friend fix_wix.cmake passes us the path to it
files_tree = ET.parse(files_path)
features_tree = ET.parse(features_path)

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

    fragment = files_root.find(".ns:Fragment", ns)
    if (fragment != None):
        schedules_dir = fragment.find("ns:DirectoryRef[@Id='CM_DP_schedules']", ns)
    else:
        return False
    # Remove 'schedules' directory
    if (schedules_dir != None):
        fragment.remove(schedules_dir)
        modified_files = modified_files and True
    return modified_files

def fix_features() -> bool:
    features_root = features_tree.getroot()

    feature_refs = features_root.findall(".//ns:FeatureRef[@Id='ProductFeature']", ns)

    for feature_ref in feature_refs:
        schedules_dir_component = feature_ref.find("ns:ComponentRef[@Id='CM_C_EMPTY_CM_DP_schedules']", ns)
        if (schedules_dir_component != None):
            feature_ref.remove(schedules_dir_component)
            return True
    return False

if (fix_files() and fix_features()):
    # Save the modified WiX files
    files_tree.write(files_path)
    features_tree.write(features_path)
    print("Python fix_wix.py: Fixed files.wxs to include AUMID and Toast Notification GUID")
    print("Python fix_wix.py: Fixed files.wxs and features_tree.wxs to remove schedules dir")