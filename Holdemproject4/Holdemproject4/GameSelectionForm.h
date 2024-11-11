#pragma once
#include "TexasHoldemForm.h"
#include "IndianPokerForm.h"
#include "MightyForm.h"
#include "ThiefCatchForm.h"
#include "OneCardForm.h"

namespace CardGameApp {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class GameSelectionForm : public System::Windows::Forms::Form
    {
    public:
        GameSelectionForm(void)
        {
            InitializeComponent();
        }

    protected:
        ~GameSelectionForm()
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        System::Windows::Forms::Button^ texasHoldemButton;
        System::Windows::Forms::Button^ indianPokerButton;
        System::Windows::Forms::Button^ mightyButton;
        System::Windows::Forms::Button^ thiefCatchButton;
        System::Windows::Forms::Button^ oneCardButton;

        System::ComponentModel::Container^ components;

        void InitializeComponent(void)
        {
            this->texasHoldemButton = (gcnew System::Windows::Forms::Button());
            this->indianPokerButton = (gcnew System::Windows::Forms::Button());
            this->mightyButton = (gcnew System::Windows::Forms::Button());
            this->thiefCatchButton = (gcnew System::Windows::Forms::Button());
            this->oneCardButton = (gcnew System::Windows::Forms::Button());
            this->SuspendLayout();
            // 
            // texasHoldemButton
            // 
            this->texasHoldemButton->Location = System::Drawing::Point(150, 500);
            this->texasHoldemButton->Name = L"texasHoldemButton";
            this->texasHoldemButton->Size = System::Drawing::Size(200, 100);
            this->texasHoldemButton->TabIndex = 0;
            this->texasHoldemButton->Text = L"텍사스 홀덤";
            this->texasHoldemButton->UseVisualStyleBackColor = true;
            this->texasHoldemButton->Click += gcnew System::EventHandler(this, &GameSelectionForm::texasHoldemButton_Click);
            // 
            // indianPokerButton
            // 
            this->indianPokerButton->Location = System::Drawing::Point(400, 500);
            this->indianPokerButton->Name = L"indianPokerButton";
            this->indianPokerButton->Size = System::Drawing::Size(200, 100);
            this->indianPokerButton->TabIndex = 1;
            this->indianPokerButton->Text = L"인디언 포커";
            this->indianPokerButton->UseVisualStyleBackColor = true;
            this->indianPokerButton->Click += gcnew System::EventHandler(this, &GameSelectionForm::indianPokerButton_Click);
            // 
            // mightyButton
            // 
            this->mightyButton->Location = System::Drawing::Point(650, 500);
            this->mightyButton->Name = L"mightyButton";
            this->mightyButton->Size = System::Drawing::Size(200, 100);
            this->mightyButton->TabIndex = 2;
            this->mightyButton->Text = L"마이티";
            this->mightyButton->UseVisualStyleBackColor = true;
            this->mightyButton->Click += gcnew System::EventHandler(this, &GameSelectionForm::mightyButton_Click);
            // 
            // thiefCatchButton
            // 
            this->thiefCatchButton->Location = System::Drawing::Point(900, 500);
            this->thiefCatchButton->Name = L"thiefCatchButton";
            this->thiefCatchButton->Size = System::Drawing::Size(200, 100);
            this->thiefCatchButton->TabIndex = 3;
            this->thiefCatchButton->Text = L"도둑잡기";
            this->thiefCatchButton->UseVisualStyleBackColor = true;
            this->thiefCatchButton->Click += gcnew System::EventHandler(this, &GameSelectionForm::thiefCatchButton_Click);
            // 
            // oneCardButton
            // 
            this->oneCardButton->Location = System::Drawing::Point(1150, 500);
            this->oneCardButton->Name = L"oneCardButton";
            this->oneCardButton->Size = System::Drawing::Size(200, 100);
            this->oneCardButton->TabIndex = 4;
            this->oneCardButton->Text = L"원카드";
            this->oneCardButton->UseVisualStyleBackColor = true;
            this->oneCardButton->Click += gcnew System::EventHandler(this, &GameSelectionForm::oneCardButton_Click);
            // 
            // GameSelectionForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(1500, 1200);
            this->Controls->Add(this->oneCardButton);
            this->Controls->Add(this->thiefCatchButton);
            this->Controls->Add(this->mightyButton);
            this->Controls->Add(this->indianPokerButton);
            this->Controls->Add(this->texasHoldemButton);
            this->Name = L"GameSelectionForm";
            this->Text = L"게임 선택";
            this->ResumeLayout(false);
        }

        void texasHoldemButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            TexasHoldemForm^ texasHoldemForm = gcnew TexasHoldemForm();
            texasHoldemForm->ShowDialog();
        }

        void indianPokerButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            IndianPokerForm^ indianPokerForm = gcnew IndianPokerForm();
            indianPokerForm->ShowDialog();
        }

        void mightyButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            MightyForm^ mightyForm = gcnew MightyForm();
            mightyForm->ShowDialog();
        }

        void thiefCatchButton_Click(Object^ sender, System::EventArgs^ e)
        {
            ThiefCatchForm^ thiefCatchForm = gcnew ThiefCatchForm();
            thiefCatchForm->ShowDialog();
        }

        void oneCardButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            OneCardForm^ oneCardForm = gcnew OneCardForm();
            oneCardForm->ShowDialog();
        }
    };
}


