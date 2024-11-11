#pragma once

namespace CardGameApp {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class OneCardForm : public System::Windows::Forms::Form
    {
    public:
        OneCardForm(void)
        {
            InitializeComponent();
        }

    protected:
        ~OneCardForm()
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
            // OneCardForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(1500, 1200);
            this->Name = L"OneCardForm";
            this->Text = L"¿øÄ«µå";
            this->ResumeLayout(false);
        }
    };
}
