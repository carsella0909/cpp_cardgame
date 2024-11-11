#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <msclr/marshal_cppstd.h>
#include <thread>
#include <chrono>

namespace CardGameApp {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace System::Collections::Generic;

    public ref class ThiefCatchForm : public System::Windows::Forms::Form
    {
    public:
        ThiefCatchForm(void)
        {
            InitializeComponent();
            InitializeGame();
        }

    protected:
        ~ThiefCatchForm()
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        System::ComponentModel::Container^ components;
        Button^ startGameButton;
        Button^ drawCardButton;
        Label^ statusLabel;
        TableLayoutPanel^ gameTableLayoutPanel;
        List<KeyValuePair<String^, String^>>^ deck;
        List<List<KeyValuePair<String^, String^>>^>^ players;
        List<bool>^ activePlayers;
        int currentPlayer;
        ListBox^ playerHandListBox;
        Label^ ai1HandCountLabel;
        Label^ ai2HandCountLabel;
        Label^ ai3HandCountLabel;
        Label^ currentPlayerLabel;

        void InitializeComponent(void)
        {
            // 
            // ThiefCatchForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(800, 600);
            this->Name = L"ThiefCatchForm";
            this->Text = L"µµµÏÀâ±â";
            this->ResumeLayout(false);

            // Initialize UI components
            InitializeUI();
        }

        void InitializeUI()
        {
            this->startGameButton = gcnew Button();
            this->drawCardButton = gcnew Button();
            this->statusLabel = gcnew Label();
            this->gameTableLayoutPanel = gcnew TableLayoutPanel();
            this->playerHandListBox = gcnew ListBox();
            this->ai1HandCountLabel = gcnew Label();
            this->ai2HandCountLabel = gcnew Label();
            this->ai3HandCountLabel = gcnew Label();
            this->currentPlayerLabel = gcnew Label(); // New label for current player

            this->startGameButton->Text = L"Start Game";
            this->startGameButton->Location = Point(10, 10);
            this->startGameButton->Click += gcnew EventHandler(this, &ThiefCatchForm::onStartGameClicked);

            this->drawCardButton->Text = L"Draw Card";
            this->drawCardButton->Location = Point(10, 420);
            this->drawCardButton->Click += gcnew EventHandler(this, &ThiefCatchForm::onDrawCardClicked);

            this->statusLabel->Text = L"Welcome to Thief Catch Game!";
            this->statusLabel->Location = Point(10, 50);
            this->statusLabel->AutoSize = true;

            this->currentPlayerLabel->Text = L"Current Player: Player";
            this->currentPlayerLabel->Location = Point(10, 80);
            this->currentPlayerLabel->AutoSize = true;

            this->playerHandListBox->Location = Point(10, 100);
            this->playerHandListBox->Size = System::Drawing::Size(200, 300);

            this->ai1HandCountLabel->Location = Point(220, 150);
            this->ai1HandCountLabel->AutoSize = true;

            this->ai2HandCountLabel->Location = Point(220, 200);
            this->ai2HandCountLabel->AutoSize = true;

            this->ai3HandCountLabel->Location = Point(220, 250);
            this->ai3HandCountLabel->AutoSize = true;

            this->Controls->Add(this->startGameButton);
            this->Controls->Add(this->drawCardButton);
            this->Controls->Add(this->statusLabel);
            this->Controls->Add(this->currentPlayerLabel); // Add new label to form
            this->Controls->Add(this->playerHandListBox);
            this->Controls->Add(this->ai1HandCountLabel);
            this->Controls->Add(this->ai2HandCountLabel);
            this->Controls->Add(this->ai3HandCountLabel);
        }

        void InitializeGame()
        {
            playerHandListBox->Items->Clear();
            players = gcnew List<List<KeyValuePair<String^, String^>>^>();
            activePlayers = gcnew List<bool>(4);
            deck = gcnew List<KeyValuePair<String^, String^>>(); // Initialize deck
            currentPlayer = 0;

            for (int i = 0; i < 4; ++i) {
                activePlayers->Add(true);
            }

            initializeDeck();
            shuffleDeck();
            dealCards();
            updateUI();
            discardInitialPairs();

            updateUI();

            statusLabel->Text = L"Game started! Find the thief!";
        }

        void initializeDeck()
        {
            array<String^>^ suits = { "H", "D", "C", "S" };
            array<String^>^ ranks = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };

            for each (String ^ suit in suits) {
                for each (String ^ rank in ranks) {
                    deck->Add(KeyValuePair<String^, String^>(suit, rank));
                }
            }

            deck->Add(KeyValuePair<String^, String^>("J", "")); // Joker
        }

        void shuffleDeck()
        {
            Random^ rnd = gcnew Random();
            for (int i = 0; i < deck->Count; i++) {
                int j = rnd->Next(i, deck->Count);
                auto temp = deck[i];
                deck[i] = deck[j];
                deck[j] = temp;
            }
        }

        void dealCards()
        {
            for (int i = 0; i < 4; ++i) {
                players->Add(gcnew List<KeyValuePair<String^, String^>>());
            }

            for (int i = 0; i < deck->Count; ++i) {
                players[i % 4]->Add(deck[i]);
            }
        }

        void discardInitialPairs()
        {
            for each (auto player in players) {
                discardPairs(player);
            }
        }

        int compareCards(KeyValuePair<String^, String^> a, KeyValuePair<String^, String^> b)
        {
            return String::Compare(a.Value, b.Value);
        }

        void onStartGameClicked(Object^ sender, EventArgs^ e)
        {
            InitializeGame();
        }

        void onDrawCardClicked(Object^ sender, EventArgs^ e)
        {
            if (currentPlayer != 0)
            {
                MessageBox::Show("It's not your turn.");
                return;
            }

            int nextPlayerIndex = getNextActivePlayer(currentPlayer);

            if (players[nextPlayerIndex]->Count == 0)
            {
                MessageBox::Show("Next AI player has no cards left.");
                return;
            }

            auto drawnCard = drawCard(players[nextPlayerIndex]);
            players[currentPlayer]->Add(drawnCard);
            discardPairs(players[currentPlayer]);

            // Notify player about the drawn card
            statusLabel->Text = L"Player drew: " + drawnCard.Key + drawnCard.Value;
            statusLabel->Refresh();
            std::this_thread::sleep_for(std::chrono::seconds(1));

            updateUI();

            if (players[nextPlayerIndex]->Count == 0)
            {
                if (nextPlayerIndex != 0) {
                    statusLabel->Text = L"AI " + nextPlayerIndex + " has finished all cards and is out of the game.";
                }
                else {
                    statusLabel->Text = L"Player"" has finished all cards and is out of the game.";
                }
                statusLabel->Refresh();
                std::this_thread::sleep_for(std::chrono::seconds(3));
                activePlayers[nextPlayerIndex] = false;
                updateUI();
            }

            // Check if the player's hand is empty
            if (players[currentPlayer]->Count == 0)
            {
                statusLabel->Text = L"Player has finished all cards and is out of the game.";
                statusLabel->Refresh();
                std::this_thread::sleep_for(std::chrono::seconds(3));
                activePlayers[currentPlayer] = false;
                updateUI();
            }

            currentPlayer = getNextActivePlayer(currentPlayer);
            updateUI(); // Update UI to show the next player
            processAITurns();
        }


        void processAITurns()
        {
            while (currentPlayer != 0)
            {
                int nextPlayerIndex = getNextActivePlayer(currentPlayer);

                if (players[nextPlayerIndex]->Count == 0)
                {
                    activePlayers[nextPlayerIndex] = false;
                    currentPlayer = nextPlayerIndex;
                    continue;
                }

                // Update UI to show the current AI player
                statusLabel->Text = L"AI " + currentPlayer + " is playing.";
                statusLabel->Refresh();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                updateUI();

                auto drawnCard = drawCard(players[nextPlayerIndex]);
                players[currentPlayer]->Add(drawnCard);
                bool discarded = discardPairs(players[currentPlayer]);

                // Notify player about the AI's discarded card if any
                if (discarded)
                {
                    statusLabel->Text = L"AI " + currentPlayer + " discarded a pair.";
                    statusLabel->Refresh();
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }

                updateUI();

                if (players[nextPlayerIndex]->Count == 0)
                {
                    if (nextPlayerIndex != 0) {
                        statusLabel->Text = L"AI " + nextPlayerIndex + " has finished all cards and is out of the game.";
                    }
                    else {
                        statusLabel->Text = L"Player"" has finished all cards and is out of the game.";
                    }
                    statusLabel->Refresh();
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    activePlayers[nextPlayerIndex] = false;
                    updateUI();
                }

                // Check if the AI's hand is empty
                if (players[currentPlayer]->Count == 0)
                {
                    statusLabel->Text = L"AI " + currentPlayer + " has finished all cards and is out of the game.";
                    statusLabel->Refresh();
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    activePlayers[currentPlayer] = false;
                }

                currentPlayer = getNextActivePlayer(currentPlayer);
                updateUI(); // Update UI to show the next player

                if (getActivePlayerCount() == 1)
                {
                    determineLoser();
                    break;
                }
            }
        }

        bool discardPairs(List<KeyValuePair<String^, String^>>^ hand)
        {
            bool discarded = false;
            hand->Sort(gcnew Comparison<KeyValuePair<String^, String^>>(this, &ThiefCatchForm::compareCards));

            for (int i = 0; i < hand->Count - 1;)
            {
                if (hand[i].Value == hand[i + 1].Value)
                {
                    hand->RemoveAt(i);
                    hand->RemoveAt(i); // Remove the next element as well
                    discarded = true;
                }
                else
                {
                    ++i;
                }
            }
            return discarded;
        }

        int getNextActivePlayer(int current)
        {
            int next = (current + 1) % 4;
            while (!activePlayers[next])
            {
                next = (next + 1) % 4;
            }
            return next;
        }

        int getActivePlayerCount()
        {
            int count = 0;
            for each (bool isActive in activePlayers)
            {
                if (isActive) count++;
            }
            return count;
        }

        void determineLoser()
        {
            for (int i = 0; i < players->Count; ++i)
            {
				if (players[0]->Count != 0)
				{
					statusLabel->Text = L"Player has the joker and loses the game!";
					statusLabel->Refresh();
					std::this_thread::sleep_for(std::chrono::seconds(3));
					break;
                }
                else {
                    if (players[i]->Count != 0)
                    {
                        statusLabel->Text = L"AI " + i + " has the Joker and loses the game.";
                        statusLabel->Refresh();
                        std::this_thread::sleep_for(std::chrono::seconds(3));
                        break;
                    }
                }
            }
        }

        KeyValuePair<String^, String^> drawCard(List<KeyValuePair<String^, String^>>^ hand)
        {
            Random^ rnd = gcnew Random();
            int index = rnd->Next(hand->Count);
            auto drawnCard = hand[index];
            hand->RemoveAt(index);

            // Notify player about the AI drawing a card from the player
            if (currentPlayer == 3)
            {
                statusLabel->Text = L"AI 3 drew: " + drawnCard.Key + drawnCard.Value + " from the player.";
                statusLabel->Refresh();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            return drawnCard;
        }

        void updateUI()
        {
            playerHandListBox->Items->Clear();
            for each (auto card in players[0]) {
                playerHandListBox->Items->Add(card.Key + card.Value);
            }

            ai1HandCountLabel->Text = L"AI 1 Hand Count: " + players[1]->Count;
            ai2HandCountLabel->Text = L"AI 2 Hand Count: " + players[2]->Count;
            ai3HandCountLabel->Text = L"AI 3 Hand Count: " + players[3]->Count;

            // Update current player label
            currentPlayerLabel->Text = L"Current Player: " + (currentPlayer == 0 ? "Player" : "AI " + currentPlayer);
            currentPlayerLabel->Refresh();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    };
}
