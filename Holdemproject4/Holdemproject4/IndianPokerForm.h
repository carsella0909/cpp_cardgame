#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>
#include <thread>
#include <chrono>

namespace CardGameApp {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Collections::Generic;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class IndianPokerForm : public System::Windows::Forms::Form
    {
    public:
        IndianPokerForm(void)
        {
            InitializeComponent();
            InitializeGame();
        }

    protected:
        ~IndianPokerForm()
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        System::ComponentModel::Container^ components;
        List<int>^ deck;
        int playerChips;
        int aiChips;
        int playerCard;
        int aiCard;
        int currentBet;
        int pot;
        int carryOverPot; // 무승부 시 이월된 칩을 저장할 변수
        bool playerActionComplete = false;

        Button^ betButton;
        Button^ foldButton;
        Button^ callButton;
        Label^ gameStateLabel;
        TextBox^ betAmountTextBox;

        void InitializeGame()
        {
            playerChips = 30;
            aiChips = 30;
            carryOverPot = 0; // 초기화
            InitializeDeck();
            ShuffleDeck();
            DealCards();
            UpdateGameState();
            std::this_thread::sleep_for(std::chrono::seconds(3));
            StartRound();
        }

        void InitializeDeck()
        {
            deck = gcnew List<int>();
            for (int i = 1; i <= 10; ++i) {
                deck->Add(i);
                deck->Add(i);
            }
        }

        void ShuffleDeck()
        {
            std::srand(unsigned(std::time(0)));
            std::vector<int> tempDeck(deck->Count);
            for (int i = 0; i < deck->Count; ++i) {
                tempDeck[i] = deck[i];
            }
            std::random_shuffle(tempDeck.begin(), tempDeck.end());
            deck->Clear();
            for (int card : tempDeck) {
                deck->Add(card);
            }
        }

        void DealCards()
        {
            playerCard = deck[deck->Count - 1];
            deck->RemoveAt(deck->Count - 1);
            aiCard = deck[deck->Count - 1];
            deck->RemoveAt(deck->Count - 1);
        }

        void StartRound()
        {
            currentBet = 0;
            pot = carryOverPot; // 이전 라운드에서 이월된 칩을 포함
            carryOverPot = 0; // 이월된 칩 초기화
            playerActionComplete = false;
            DealCards();
            UpdateGameState();
            PlayerAction();
        }

        void PlayerAction()
        {
            gameStateLabel->Text = "Your turn. Place your bet or fold." + "\nPlayer chips: " + playerChips + "\nAI chips: " + aiChips + "\nPot: " + pot + "\nAI card: " + aiCard;
            gameStateLabel->Refresh();
            betButton->Enabled = true;
            foldButton->Enabled = true;
            callButton->Enabled = true;
        }

        void OnBetButtonClick(Object^ sender, EventArgs^ e)
        {
            int betAmount = Int32::Parse(betAmountTextBox->Text);
            if (betAmount > playerChips) {
                MessageBox::Show("You don't have enough chips.");
                return;
            }
            playerChips -= betAmount;
            pot += betAmount;
            currentBet = betAmount;
            playerActionComplete = true;
            AIAction();
        }

        void OnFoldButtonClick(Object^ sender, EventArgs^ e)
        {
            aiChips += pot;
            playerActionComplete = true;
            CheckGameOver();
        }

        void OnCallButtonClick(Object^ sender, EventArgs^ e)
        {
            int callAmount = currentBet;
            if (callAmount > playerChips) {
                MessageBox::Show("You don't have enough chips.");
                return;
            }
            playerChips -= callAmount;
            pot += callAmount;
            playerActionComplete = true;
            DetermineWinner();
        }

        void AIAction()
        {
            gameStateLabel->Text = "AI is thinking...";
            gameStateLabel->Refresh(); // 강제로 다시 그리기
            std::this_thread::sleep_for(std::chrono::seconds(1)); // 1초 대기

            if (rand() % 2 == 0) {
                int callAmount = currentBet;
                if (callAmount > aiChips) {
                    aiChips = 0;
                }
                else {
                    aiChips -= callAmount;
                }
                pot += callAmount;
                gameStateLabel->Text = "AI calls.";
                gameStateLabel->Refresh(); // 강제로 다시 그리기
                std::this_thread::sleep_for(std::chrono::seconds(2));
                DetermineWinner();
            }
            else {
                int betAmount = currentBet + (rand() % 5 + 1);
                if (betAmount > aiChips) {
                    betAmount = aiChips;
                }
                aiChips -= betAmount;
                pot += betAmount;
                currentBet = betAmount;
                gameStateLabel->Text = "AI raises to " + betAmount + " chips.";
                gameStateLabel->Refresh(); // 강제로 다시 그리기
                std::this_thread::sleep_for(std::chrono::seconds(2));
                PlayerAction();
            }
        }

        void DetermineWinner()
        {
            if (playerCard > aiCard) {
                playerChips += pot;
                gameStateLabel->Text = "You win this round!";
                gameStateLabel->Refresh();
            }
            else if (playerCard < aiCard) {
                aiChips += pot;
                gameStateLabel->Text = "AI wins this round!";
                gameStateLabel->Refresh();
            }
            else {
                carryOverPot = pot; // 무승부 시 칩을 이월
                gameStateLabel->Text = "It's a tie! Pot carries over to the next round.";
                gameStateLabel->Refresh();
            }
            std::this_thread::sleep_for(std::chrono::seconds(3)); // 3초 대기
            CheckGameOver();
        }

        void CheckGameOver()
        {
            String^ state1 = "Player card: " + playerCard + "\nAI card: " + aiCard;
            gameStateLabel->Text = state1;
            gameStateLabel->Refresh();
            std::this_thread::sleep_for(std::chrono::seconds(3));
            if (playerChips <= 0) {
                MessageBox::Show("You have lost all your chips. Game over.");
                this->Close();
            }
            else if (aiChips <= 0) {
                MessageBox::Show("AI has lost all its chips. You win!");
                this->Close();
            }
            else {
                StartRound();
            }
        }

        void UpdateGameState()
        {
            String^ state = "Player chips: " + playerChips + "\nAI chips: " + aiChips + "\nPot: " + pot + "\nAI card: " + aiCard;
            gameStateLabel->Text = state;
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }

        void InitializeComponent(void)
        {
            this->betButton = gcnew Button();
            this->foldButton = gcnew Button();
            this->callButton = gcnew Button();
            this->betAmountTextBox = gcnew TextBox();
            this->gameStateLabel = gcnew Label();

            // 
            // betButton
            // 
            this->betButton->Location = System::Drawing::Point(50, 500);
            this->betButton->Name = L"betButton";
            this->betButton->Size = System::Drawing::Size(75, 23);
            this->betButton->Text = L"Bet";
            this->betButton->UseVisualStyleBackColor = true;
            this->betButton->Click += gcnew System::EventHandler(this, &IndianPokerForm::OnBetButtonClick);

            // 
            // foldButton
            // 
            this->foldButton->Location = System::Drawing::Point(150, 500);
            this->foldButton->Name = L"foldButton";
            this->foldButton->Size = System::Drawing::Size(75, 23);
            this->foldButton->Text = L"Fold";
            this->foldButton->UseVisualStyleBackColor = true;
            this->foldButton->Click += gcnew System::EventHandler(this, &IndianPokerForm::OnFoldButtonClick);

            // 
            // callButton
            // 
            this->callButton->Location = System::Drawing::Point(250, 500);
            this->callButton->Name = L"callButton";
            this->callButton->Size = System::Drawing::Size(75, 23);
            this->callButton->Text = L"Call";
            this->callButton->UseVisualStyleBackColor = true;
            this->callButton->Click += gcnew System::EventHandler(this, &IndianPokerForm::OnCallButtonClick);

            // 
            // betAmountTextBox
            // 
            this->betAmountTextBox->Location = System::Drawing::Point(50, 450);
            this->betAmountTextBox->Name = L"betAmountTextBox";
            this->betAmountTextBox->Size = System::Drawing::Size(100, 22);

            // 
            // gameStateLabel
            // 
            this->gameStateLabel->Location = System::Drawing::Point(50, 50);
            this->gameStateLabel->Name = L"gameStateLabel";
            this->gameStateLabel->Size = System::Drawing::Size(700, 400);
            this->gameStateLabel->Text = L"Game State";

            // 
            // IndianPokerForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(800, 600);
            this->Controls->Add(this->betButton);
            this->Controls->Add(this->foldButton);
            this->Controls->Add(this->callButton);
            this->Controls->Add(this->betAmountTextBox);
            this->Controls->Add(this->gameStateLabel);
            this->Name = L"IndianPokerForm";
            this->Text = L"인디언 포커";
            this->ResumeLayout(false);
            this->PerformLayout();
        }
    };
}
