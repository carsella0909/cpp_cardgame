#pragma once

namespace CardGameApp {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class SettingsForm : public System::Windows::Forms::Form
    {
    public:
        SettingsForm(void)
        {
            InitializeComponent();
        }

    protected:
        ~SettingsForm()
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        System::ComponentModel::Container^ components;

        void InitializeComponent(void)
        {
            this->SuspendLayout();
            // 
            // SettingsForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(1500, 1200);
            this->Name = L"SettingsForm";
            this->Text = L"����";
            this->ResumeLayout(false);
        }
    };
}
