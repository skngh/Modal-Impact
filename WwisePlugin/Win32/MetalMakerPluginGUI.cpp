/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#include "MetalMakerPluginGUI.h"
// #include "../resource.h"

MetalMakerPluginGUI::MetalMakerPluginGUI()
{
}

// Determine what dialog just get called and set the property names to UI control binding populated table.
// bool MetalMakerPluginGUI::GetDialog(AK::Wwise::Plugin::eDialog in_eDialog, UINT &out_uiDialogID, AK::Wwise::Plugin::PopulateTableItem *&out_pTable) const
// {
//   switch (in_eDialog)
//   {
//   case AK::Wwise::Plugin::SettingsDialog:
//     out_uiDialogID = IDD_METALMAKERPLUGIN_BIG;
//     out_pTable = NULL;
//     return true;

//   case AK::Wwise::Plugin::ContentsEditorDialog:
//     out_uiDialogID = IDD_METALMAKERPLUGIN_SMALL;
//     out_pTable = NULL;
//     return true;
//   }

//   return false;
// }

AK_ADD_PLUGIN_CLASS_TO_CONTAINER(
    MetalMaker,          // Name of the plug-in container for this shared library
    MetalMakerPluginGUI, // Authoring plug-in class to add to the plug-in container
    MetalMakerSource     // Corresponding Sound Engine plug-in class
);
