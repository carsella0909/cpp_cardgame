#pragma once

namespace CardGameApp {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class LoadGameForm : public System::Windows::Forms::Form
    {
    public:
        LoadGameForm(void)
        {
            InitializeComponent();
        }

    protected:
        ~LoadGameForm()
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
            // LoadGameForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(1500, 1200);
            this->Name = L"LoadGameForm";
            this->Text = L"게임 소개";
            this->ResumeLayout(false);
        }
    };
}
