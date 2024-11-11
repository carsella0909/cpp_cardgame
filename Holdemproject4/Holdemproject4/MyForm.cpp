#include "MyForm.h"
#include "MainForm.h"
#include "GameSelectionForm.h"
#include "LoadGameForm.h"
#include "SettingsForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
void main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    CardGameApp::MainForm form;
    Application::Run(% form);
}