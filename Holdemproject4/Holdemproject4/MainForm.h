#pragma once
#include "GameSelectionForm.h"
#include "LoadGameForm.h"
#include "SettingsForm.h"

namespace CardGameApp {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class MainForm : public System::Windows::Forms::Form
    {
    public:
        MainForm(void)
        {
            InitializeComponent();
        }

    protected:
        ~MainForm()
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        System::Windows::Forms::Button^ startButton;
        System::Windows::Forms::Button^ loadButton;
        System::Windows::Forms::Button^ settingsButton;
        System::Windows::Forms::Label^ titleLabel;

        System::ComponentModel::Container^ components;

        void InitializeComponent(void)
        {
            this->startButton = (gcnew System::Windows::Forms::Button());
            this->loadButton = (gcnew System::Windows::Forms::Button());
            this->settingsButton = (gcnew System::Windows::Forms::Button());
            this->titleLabel = (gcnew System::Windows::Forms::Label());
            this->SuspendLayout();
            // 
            // startButton
            // 
            this->startButton->Location = System::Drawing::Point(650, 400);
            this->startButton->Name = L"startButton";
            this->startButton->Size = System::Drawing::Size(200, 100);
            this->startButton->TabIndex = 0;
            this->startButton->Text = L"시작하기";
            this->startButton->UseVisualStyleBackColor = false;
            this->startButton->BackColor = System::Drawing::Color::DarkGreen;
            this->startButton->ForeColor = System::Drawing::Color::White;
            this->startButton->Font = (gcnew System::Drawing::Font(L"Arial", 14, System::Drawing::FontStyle::Bold));
            this->startButton->Click += gcnew System::EventHandler(this, &MainForm::startButton_Click);
            // 
            // loadButton
            // 
            this->loadButton->Location = System::Drawing::Point(650, 550);
            this->loadButton->Name = L"loadButton";
            this->loadButton->Size = System::Drawing::Size(200, 100);
            this->loadButton->TabIndex = 1;
            this->loadButton->Text = L"불러오기";
            this->loadButton->UseVisualStyleBackColor = false;
            this->loadButton->BackColor = System::Drawing::Color::DarkBlue;
            this->loadButton->ForeColor = System::Drawing::Color::White;
            this->loadButton->Font = (gcnew System::Drawing::Font(L"Arial", 14, System::Drawing::FontStyle::Bold));
            this->loadButton->Click += gcnew System::EventHandler(this, &MainForm::loadButton_Click);
            // 
            // settingsButton
            // 
            this->settingsButton->Location = System::Drawing::Point(650, 700);
            this->settingsButton->Name = L"settingsButton";
            this->settingsButton->Size = System::Drawing::Size(200, 100);
            this->settingsButton->TabIndex = 2;
            this->settingsButton->Text = L"설정";
            this->settingsButton->UseVisualStyleBackColor = false;
            this->settingsButton->BackColor = System::Drawing::Color::DarkRed;
            this->settingsButton->ForeColor = System::Drawing::Color::White;
            this->settingsButton->Font = (gcnew System::Drawing::Font(L"Arial", 14, System::Drawing::FontStyle::Bold));
            this->settingsButton->Click += gcnew System::EventHandler(this, &MainForm::settingsButton_Click);
            // 
            // titleLabel
            // 
            this->titleLabel->AutoSize = true;
            this->titleLabel->Font = (gcnew System::Drawing::Font(L"Arial", 50, System::Drawing::FontStyle::Bold));
            this->titleLabel->Location = System::Drawing::Point(480, 150);
            this->titleLabel->Name = L"titleLabel";
            this->titleLabel->Size = System::Drawing::Size(500, 70);
            this->titleLabel->TabIndex = 3;
            this->titleLabel->Text = L"트럼프 카드 게임";
            this->titleLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
            // 
            // MainForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(1500, 1200); // 창 크기를 1500x1200으로 설정
            this->BackColor = System::Drawing::Color::LightBlue; // 배경 색상 설정
            this->Controls->Add(this->titleLabel);
            this->Controls->Add(this->settingsButton);
            this->Controls->Add(this->loadButton);
            this->Controls->Add(this->startButton);
            this->Name = L"MainForm";
            this->Text = L"트럼프 카드 게임";
            this->ResumeLayout(false);
            this->PerformLayout();
        }

        void startButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            GameSelectionForm^ gameSelectionForm = gcnew GameSelectionForm();
            gameSelectionForm->ShowDialog();
        }

        void loadButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            LoadGameForm^ loadGameForm = gcnew LoadGameForm();
            loadGameForm->ShowDialog();
        }

        void settingsButton_Click(System::Object^ sender, System::EventArgs^ e)
        {
            SettingsForm^ settingsForm = gcnew SettingsForm();
            settingsForm->ShowDialog();
        }
    };
}
