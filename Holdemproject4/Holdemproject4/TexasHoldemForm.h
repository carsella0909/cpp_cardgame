#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <string>
#include <msclr\marshal_cppstd.h>
#include <thread>
#include <chrono>

namespace CardGameApp {
    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class Card2 {
    public:
        enum class Suit { HEARTS, DIAMONDS, CLUBS, SPADES };
        enum class Rank { TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };

        Card2(Rank r, Suit s) : rank(r), suit(s) {}

        Rank getRank() { return rank; }
        Suit getSuit() { return suit; }

        String^ toString() {
            String^ rankStr;
            switch (rank) {
            case Rank::JACK: rankStr = "J"; break;
            case Rank::QUEEN: rankStr = "Q"; break;
            case Rank::KING: rankStr = "K"; break;
            case Rank::ACE: rankStr = "A"; break;
            default: rankStr = ((int)rank).ToString();
            }

            String^ suitStr;
            switch (suit) {
            case Suit::HEARTS: suitStr = "♥"; break;
            case Suit::DIAMONDS: suitStr = "◆"; break;
            case Suit::CLUBS: suitStr = "♣"; break;
            case Suit::SPADES: suitStr = "♠"; break;
            }

            return rankStr + suitStr;
        }

    private:
        Rank rank;
        Suit suit;
    };

    public ref class Deck2 {
    public:
        Deck2() {
            for (int s = 0; s < 4; ++s) {
                for (int r = 2; r <= 14; ++r) {
                    cards->Add(gcnew Card2((Card2::Rank)r, (Card2::Suit)s));
                }
            }
            shuffle();
        }

        void shuffle() {
            Random^ rng = gcnew Random();
            int n = cards->Count;
            while (n > 1) {
                n--;
                int k = rng->Next(n + 1);
                Card2^ value = cards[k];
                cards[k] = cards[n];
                cards[n] = value;
            }
        }

        Card2^ drawCard() {
            if (cards->Count == 0) return nullptr;
            Card2^ card = cards[cards->Count - 1];
            cards->RemoveAt(cards->Count - 1);
            return card;
        }

    private:
        System::Collections::Generic::List<Card2^>^ cards = gcnew System::Collections::Generic::List<Card2^>();
    };

    public ref class Player2 {
    public:
        Player2(String^ n, int c, bool isAI) : name(n), chips(c), isAI(isAI), currentBet(0), folded(false) {}

        void setHoleCards(Card2^ c1, Card2^ c2) {
            holeCards[0] = c1;
            holeCards[1] = c2;
        }

        String^ getName() { return name; }
        int getChips() { return chips; }
        array<Card2^>^ getHoleCards() { return holeCards; }
        bool isAIPlayer() { return isAI; }

        void bet(int amount) {
            chips -= amount;
            currentBet += amount;
        }

        void addChips(int amount) {
            chips += amount;
        }

        void resetBet() { currentBet = 0; }
        int getCurrentBet() { return currentBet; }
        void setFolded(bool f) { folded = f; }
        bool hasFolded() { return folded; }

    private:
        String^ name;
        int chips;
        array<Card2^>^ holeCards = gcnew array<Card2^>(2);
        bool isAI;
        int currentBet;
        bool folded;
    };

    public ref class PokerTable {
    public:
        PokerTable() : currentPlayer(0), pot(0), currentBet(0), dealerPosition(0), smallBlind(5), bigBlind(10) {
            deck = gcnew Deck2();
        }

        void addPlayer(Player2^ player) {
            players->Add(player);
        }

        void dealHoleCards() {
            for each (Player2 ^ player in players) {
                player->setHoleCards(deck->drawCard(), deck->drawCard());
            }
        }

        void dealFlop() {
            deck->drawCard(); // Burn card
            for (int i = 0; i < 3; ++i) {
                communityCards->Add(deck->drawCard());
            }
        }

        void dealTurn() {
            deck->drawCard(); // Burn card
            communityCards->Add(deck->drawCard());
        }

        void dealRiver() {
            deck->drawCard(); // Burn card
            communityCards->Add(deck->drawCard());
        }

        void startNewHand() {
            deck = gcnew Deck2();
            communityCards->Clear();
            pot = 0;
            currentBet = 0;
            for each (Player2 ^ player in players) {
                player->resetBet();
                player->setFolded(false);
            }
            dealerPosition = (dealerPosition + 1) % players->Count;
            currentPlayer = (dealerPosition + 3) % players->Count; // Start with player after big blind
            postBlinds();
            dealHoleCards();
        }

        void postBlinds() {
            int smallBlindPos = (dealerPosition + 1) % players->Count;
            int bigBlindPos = (dealerPosition + 2) % players->Count;
            players[smallBlindPos]->bet(smallBlind);
            players[bigBlindPos]->bet(bigBlind);
            pot += smallBlind + bigBlind;
            currentBet = bigBlind;
        }

        bool playRound() {
            int playersInHand = players->Count;
            int consecutiveChecks = 0;

            while (playersInHand > 1 && consecutiveChecks < players->Count) {
                Player2^ currentPlayerObj = players[currentPlayer];
                if (!currentPlayerObj->hasFolded()) {
                    int action = getPlayerAction(currentPlayerObj);
                    switch (action) {
                    case 0: // Fold
                        currentPlayerObj->setFolded(true);
                        playersInHand--;
                        consecutiveChecks = 0;
                        break;
                    case 1: // Call
                    {
                        int callAmount = currentBet - currentPlayerObj->getCurrentBet();
                        currentPlayerObj->bet(callAmount);
                        pot += callAmount;
                        consecutiveChecks = 0;
                    }
                    break;
                    case 2: // Raise
                    {
                        int raiseAmount = currentBet * 2 - currentPlayerObj->getCurrentBet();
                        currentPlayerObj->bet(raiseAmount);
                        pot += raiseAmount;
                        currentBet *= 2;
                        consecutiveChecks = 0;
                    }
                    break;
                    case 3: // Check
                        consecutiveChecks++;
                        break;
                    }
                }
                currentPlayer = (currentPlayer + 1) % players->Count;
            }

            return playersInHand > 1;
        }

        int evaluateHand(System::Collections::Generic::List<Card2^>^ cards) {
            if (isRoyalFlush(cards)) return 10;
            if (isStraightFlush(cards)) return 9;
            if (isFourOfAKind(cards)) return 8;
            if (isFullHouse(cards)) return 7;
            if (isFlush(cards)) return 6;
            if (isStraight(cards)) return 5;
            if (isThreeOfAKind(cards)) return 4;
            if (isTwoPair(cards)) return 3;
            if (isOnePair(cards)) return 2;
            return getHighCard(cards);
        }

        bool isFlush(System::Collections::Generic::List<Card2^>^ cards) {
            // 플러시인지 확인하는 로직을 구현합니다.
            // 예제에서는 간단히 모든 카드의 슈트가 같은지 확인합니다.
            auto suit = cards[0]->getSuit();
            for each (Card2 ^ card in cards) {
                if (card->getSuit() != suit) return false;
            }
            return true;
        }

        bool isStraight(System::Collections::Generic::List<Card2^>^ cards) {
            // 스트레이트인지 확인하는 로직을 구현합니다.
            cards->Sort(gcnew Comparison<Card2^>(&PokerTable::CompareCards));
            for (int i = 1; i < cards->Count; ++i) {
                if ((int)cards[i]->getRank() != (int)cards[i - 1]->getRank() + 1) return false;
            }
            return true;
        }

        bool isRoyalFlush(System::Collections::Generic::List<Card2^>^ cards) {
            // 로얄 스트레이트 플러시인지 확인하는 로직을 구현합니다.
            return isFlush(cards) && isStraight(cards) && cards[0]->getRank() == Card2::Rank::TEN;
        }

        bool isStraightFlush(System::Collections::Generic::List<Card2^>^ cards) {
            // 스트레이트 플러시인지 확인하는 로직을 구현합니다.
            return isFlush(cards) && isStraight(cards);
        }

        bool isFourOfAKind(System::Collections::Generic::List<Card2^>^ cards) {
            // 포카드인지 확인하는 로직을 구현합니다.
            auto rankCounts = gcnew System::Collections::Generic::Dictionary<Card2::Rank, int>();
            for each (Card2 ^ card in cards) {
                if (!rankCounts->ContainsKey(card->getRank())) {
                    rankCounts[card->getRank()] = 0;
                }
                rankCounts[card->getRank()]++;
            }
            for each (auto kvp in rankCounts) {
                if (kvp.Value == 4) return true;
            }
            return false;
        }

        bool isFullHouse(System::Collections::Generic::List<Card2^>^ cards) {
            // 풀하우스인지 확인하는 로직을 구현합니다.
            auto rankCounts = gcnew System::Collections::Generic::Dictionary<Card2::Rank, int>();
            for each (Card2 ^ card in cards) {
                if (!rankCounts->ContainsKey(card->getRank())) {
                    rankCounts[card->getRank()] = 0;
                }
                rankCounts[card->getRank()]++;
            }
            bool hasThree = false;
            bool hasTwo = false;
            for each (auto kvp in rankCounts) {
                if (kvp.Value == 3) hasThree = true;
                if (kvp.Value == 2) hasTwo = true;
            }
            return hasThree && hasTwo;
        }

        bool isThreeOfAKind(System::Collections::Generic::List<Card2^>^ cards) {
            // 쓰리 오브 어 카인드인지 확인하는 로직을 구현합니다.
            auto rankCounts = gcnew System::Collections::Generic::Dictionary<Card2::Rank, int>();
            for each (Card2 ^ card in cards) {
                if (!rankCounts->ContainsKey(card->getRank())) {
                    rankCounts[card->getRank()] = 0;
                }
                rankCounts[card->getRank()]++;
            }
            for each (auto kvp in rankCounts) {
                if (kvp.Value == 3) return true;
            }
            return false;
        }

        bool isTwoPair(System::Collections::Generic::List<Card2^>^ cards) {
            // 투페어인지 확인하는 로직을 구현합니다.
            auto rankCounts = gcnew System::Collections::Generic::Dictionary<Card2::Rank, int>();
            for each (Card2 ^ card in cards) {
                if (!rankCounts->ContainsKey(card->getRank())) {
                    rankCounts[card->getRank()] = 0;
                }
                rankCounts[card->getRank()]++;
            }
            int pairCount = 0;
            for each (auto kvp in rankCounts) {
                if (kvp.Value == 2) pairCount++;
            }
            return pairCount == 2;
        }

        bool isOnePair(System::Collections::Generic::List<Card2^>^ cards) {
            // 원페어인지 확인하는 로직을 구현합니다.
            auto rankCounts = gcnew System::Collections::Generic::Dictionary<Card2::Rank, int>();
            for each (Card2 ^ card in cards) {
                if (!rankCounts->ContainsKey(card->getRank())) {
                    rankCounts[card->getRank()] = 0;
                }
                rankCounts[card->getRank()]++;
            }
            for each (auto kvp in rankCounts) {
                if (kvp.Value == 2) return true;
            }
            return false;
        }

        int getHighCard(System::Collections::Generic::List<Card2^>^ cards) {
            // 하이카드를 반환하는 로직을 구현합니다.
            int highestRank = 0;
            for each (Card2 ^ card in cards) {
                if ((int)card->getRank() > highestRank) {
                    highestRank = (int)card->getRank();
                }
            }
            return highestRank;
        }

        Player2^ determineWinner() {
            Player2^ winner = nullptr;
            int highestRank = -1;

            for each (Player2 ^ player in players) {
                if (!player->hasFolded()) {
                    System::Collections::Generic::List<Card2^>^ allCards = gcnew System::Collections::Generic::List<Card2^>();
                    for each (Card2 ^ card in player->getHoleCards()) {
                        allCards->Add(card);
                    }
                    for each (Card2 ^ card in communityCards) {
                        allCards->Add(card);
                    }

                    System::Collections::Generic::List<System::Collections::Generic::List<Card2^>^>^ allCombinations = gcnew System::Collections::Generic::List<System::Collections::Generic::List<Card2^>^>();
                    System::Collections::Generic::List<Card2^>^ current = gcnew System::Collections::Generic::List<Card2^>();
                    getCombinations(allCards, 0, 0, current, allCombinations);

                    int bestHandRank = -1;
                    for each (System::Collections::Generic::List<Card2^> ^ combination in allCombinations) {
                        int handRank = evaluateHand(combination);
                        if (handRank > bestHandRank) {
                            bestHandRank = handRank;
                        }
                    }

                    if (bestHandRank > highestRank) {
                        highestRank = bestHandRank;
                        winner = player;
                    }
                }
            }

            return winner;
        }

        void getCombinations(System::Collections::Generic::List<Card2^>^ cards, int start, int depth, System::Collections::Generic::List<Card2^>^ current, System::Collections::Generic::List<System::Collections::Generic::List<Card2^>^>^ allCombinations) {
            if (depth == 5) {
                allCombinations->Add(gcnew System::Collections::Generic::List<Card2^>(current));
                return;
            }
            for (int i = start; i < cards->Count; ++i) {
                current->Add(cards[i]);
                getCombinations(cards, i + 1, depth + 1, current, allCombinations);
                current->RemoveAt(current->Count - 1);
            }
        }


        void distributePot(Player2^ winner) {
            winner->addChips(pot);
            pot = 0;
        }

        int getPlayerAction(Player2^ player) {
            int playerAction = -1;
            if (player->isAIPlayer()) {
                // Implement AI logic here
                Random^ rng = gcnew Random();
                return rng->Next(4); // Random action for now
            }
            else {
                // Wait for human player action
                while (playerAction == -1) {
                    Application::DoEvents(); // Process UI events
                }
                int action = playerAction;
                playerAction = -1; // Reset action
                return action;
            }
        }

        System::Collections::Generic::List<Player2^>^ getPlayers() { return players; }
        System::Collections::Generic::List<Card2^>^ getCommunityCards() { return communityCards; }
        int getPot() { return pot; }
        int getCurrentBet() { return currentBet; }

    private:
        System::Collections::Generic::List<Player2^>^ players = gcnew System::Collections::Generic::List<Player2^>();
        System::Collections::Generic::List<Card2^>^ communityCards = gcnew System::Collections::Generic::List<Card2^>();
        Deck2^ deck;
        int currentPlayer;
        int pot;
        int currentBet;
        int dealerPosition;
        int smallBlind;
        int bigBlind;
        static int CompareCards(Card2^ a, Card2^ b) {
            return (int)a->getRank() - (int)b->getRank();
        }
    };

    public ref class TexasHoldemForm : public System::Windows::Forms::Form {
    public:
        TexasHoldemForm(void) {
            InitializeComponent();
            table = gcnew PokerTable();
            table->addPlayer(gcnew Player2("Player 1", 1000, 0));
            table->addPlayer(gcnew Player2("AI 1", 1000, 1));
            table->addPlayer(gcnew Player2("AI 2", 1000, 1));
            table->addPlayer(gcnew Player2("AI 3", 1000, 1));
            UpdateDisplay();
        }

    protected:
        ~TexasHoldemForm() {
            if (components) {
                delete components;
            }
        }

    private:
        PokerTable^ table;
        System::Windows::Forms::Button^ dealButton;
        System::Windows::Forms::Button^ foldButton;
        System::Windows::Forms::Button^ checkButton;
        System::Windows::Forms::Button^ callButton;
        System::Windows::Forms::Button^ raiseButton;
        System::Windows::Forms::Label^ communityCardsLabel;
        System::Windows::Forms::Label^ player_Label;
        System::Windows::Forms::Label^ AI1Label;
        System::Windows::Forms::Label^ AI2Label;
        System::Windows::Forms::Label^ AI3Label;
        System::Windows::Forms::Label^ potLabel;
        System::ComponentModel::Container^ components;
        int playerAction;

        void InitializeComponent() {
            this->dealButton = (gcnew System::Windows::Forms::Button());
            this->foldButton = (gcnew System::Windows::Forms::Button());
            this->checkButton = (gcnew System::Windows::Forms::Button());
            this->callButton = (gcnew System::Windows::Forms::Button());
            this->raiseButton = (gcnew System::Windows::Forms::Button());
            this->communityCardsLabel = (gcnew System::Windows::Forms::Label());
            this->player_Label = (gcnew System::Windows::Forms::Label());
            this->AI1Label = (gcnew System::Windows::Forms::Label());
            this->AI2Label = (gcnew System::Windows::Forms::Label());
            this->AI3Label = (gcnew System::Windows::Forms::Label());
            this->potLabel = (gcnew System::Windows::Forms::Label());
            this->SuspendLayout();

            // dealButton
            this->dealButton->Location = System::Drawing::Point(12, 12);
            this->dealButton->Name = L"dealButton";
            this->dealButton->Size = System::Drawing::Size(75, 23);
            this->dealButton->TabIndex = 0;
            this->dealButton->Text = L"Deal";
            this->dealButton->UseVisualStyleBackColor = true;
            this->dealButton->Click += gcnew System::EventHandler(this, &TexasHoldemForm::dealButton_Click);

            // foldButton
            this->foldButton->Location = System::Drawing::Point(12, 240);
            this->foldButton->Name = L"foldButton";
            this->foldButton->Size = System::Drawing::Size(75, 23);
            this->foldButton->TabIndex = 1;
            this->foldButton->Text = L"Fold";
            this->foldButton->UseVisualStyleBackColor = true;
            this->foldButton->Click += gcnew System::EventHandler(this, &TexasHoldemForm::foldButton_Click);

            // checkButton
            this->checkButton->Location = System::Drawing::Point(93, 240);
            this->checkButton->Name = L"checkButton";
            this->checkButton->Size = System::Drawing::Size(75, 23);
            this->checkButton->TabIndex = 2;
            this->checkButton->Text = L"Check";
            this->checkButton->UseVisualStyleBackColor = true;
            this->checkButton->Click += gcnew System::EventHandler(this, &TexasHoldemForm::checkButton_Click);

            // callButton
            this->callButton->Location = System::Drawing::Point(174, 240);
            this->callButton->Name = L"callButton";
            this->callButton->Size = System::Drawing::Size(75, 23);
            this->callButton->TabIndex = 3;
            this->callButton->Text = L"Call";
            this->callButton->UseVisualStyleBackColor = true;
            this->callButton->Click += gcnew System::EventHandler(this, &TexasHoldemForm::callButton_Click);

            // raiseButton
            this->raiseButton->Location = System::Drawing::Point(255, 240);
            this->raiseButton->Name = L"raiseButton";
            this->raiseButton->Size = System::Drawing::Size(75, 23);
            this->raiseButton->TabIndex = 4;
            this->raiseButton->Text = L"Raise";
            this->raiseButton->UseVisualStyleBackColor = true;
            this->raiseButton->Click += gcnew System::EventHandler(this, &TexasHoldemForm::raiseButton_Click);

            // communityCardsLabel
            this->communityCardsLabel->AutoSize = true;
            this->communityCardsLabel->Location = System::Drawing::Point(12, 50);
            this->communityCardsLabel->Name = L"communityCardsLabel";
            this->communityCardsLabel->Size = System::Drawing::Size(100, 13);
            this->communityCardsLabel->TabIndex = 5;
            this->communityCardsLabel->Text = L"Community Cards:";

            // playerLabel
            this->player_Label->AutoSize = true;
            this->player_Label->Location = System::Drawing::Point(12, 80);
            this->player_Label->Name = L"playerLabel";
            this->player_Label->Size = System::Drawing::Size(35, 13);
            this->player_Label->TabIndex = 6;
            this->player_Label->Text = L"Player:";

            // AI1Label
            this->AI1Label->AutoSize = true;
            this->AI1Label->Location = System::Drawing::Point(12, 110);
            this->AI1Label->Name = L"AI1Label";
            this->AI1Label->Size = System::Drawing::Size(35, 13);
            this->AI1Label->TabIndex = 7;
            this->AI1Label->Text = L"AI 1:";

            // AI2Label
            this->AI2Label->AutoSize = true;
            this->AI2Label->Location = System::Drawing::Point(12, 140);
            this->AI2Label->Name = L"AI2Label";
            this->AI2Label->Size = System::Drawing::Size(35, 13);
            this->AI2Label->TabIndex = 8;
            this->AI2Label->Text = L"AI 2:";

            // AI3Label
            this->AI3Label->AutoSize = true;
            this->AI3Label->Location = System::Drawing::Point(12, 170);
            this->AI3Label->Name = L"AI3Label";
            this->AI3Label->Size = System::Drawing::Size(35, 13);
            this->AI3Label->TabIndex = 9;
            this->AI3Label->Text = L"AI 3:";

            // potLabel
            this->potLabel->AutoSize = true;
            this->potLabel->Location = System::Drawing::Point(12, 200);
            this->potLabel->Name = L"potLabel";
            this->potLabel->Size = System::Drawing::Size(35, 13);
            this->potLabel->TabIndex = 10;
            this->potLabel->Text = L"Pot:";

            // TexasHoldemForm
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(344, 281);
            this->Controls->Add(this->potLabel);
            this->Controls->Add(this->AI3Label);
            this->Controls->Add(this->AI2Label);
            this->Controls->Add(this->AI1Label);
            this->Controls->Add(this->player_Label);
            this->Controls->Add(this->communityCardsLabel);
            this->Controls->Add(this->raiseButton);
            this->Controls->Add(this->callButton);
            this->Controls->Add(this->checkButton);
            this->Controls->Add(this->foldButton);
            this->Controls->Add(this->dealButton);
            this->Name = L"TexasHoldemForm";
            this->Text = L"Texas Hold'em";
            this->ResumeLayout(false);
            this->PerformLayout();
        }

        System::Void dealButton_Click(System::Object^ sender, System::EventArgs^ e) {
            table->startNewHand();
            UpdateDisplay();
            if (PlayRoundWithUpdates()) { // Pre-flop
                UpdateDisplay();
                table->dealFlop();
                UpdateDisplay();
                if (PlayRoundWithUpdates()) { // Flop
                    UpdateDisplay();
                    table->dealTurn();
                    UpdateDisplay();
                    if (PlayRoundWithUpdates()) { // Turn
                        UpdateDisplay();
                        table->dealRiver();
                        UpdateDisplay();
                        PlayRoundWithUpdates(); // River
                        UpdateDisplay();
                    }
                }
            }
            // Determine winner and distribute pot here
            Player2^ winner = table->determineWinner();
            table->distributePot(winner);
            UpdateDisplay();
        }

        System::Void foldButton_Click(System::Object^ sender, System::EventArgs^ e) {
            playerAction = 0; // Fold
            UpdateDisplay();
        }

        System::Void checkButton_Click(System::Object^ sender, System::EventArgs^ e) {
            playerAction = 1; // Check
            UpdateDisplay();
        }

        System::Void callButton_Click(System::Object^ sender, System::EventArgs^ e) {
            playerAction = 2; // Call
            UpdateDisplay();
        }

        System::Void raiseButton_Click(System::Object^ sender, System::EventArgs^ e) {
            playerAction = 3; // Raise
            UpdateDisplay();
        }

        bool PlayRoundWithUpdates() {
            bool result = table->playRound();
            UpdateDisplay(); // Update after each round of AI actions
            return result;
        }

        void UpdateDisplay() {
            // Update community cards
            String^ communityCardsStr = "Community Cards: ";
            for each (Card2 ^ card in table->getCommunityCards()) {
                communityCardsStr += card->toString() + " ";
            }
            this->communityCardsLabel->Text = communityCardsStr;
            communityCardsLabel->Refresh();

            // Update player information
            auto players = table->getPlayers();
            for (int i = 0; i < players->Count; ++i) {
                String^ playerStr = players[i]->getName() + ": $" + players[i]->getChips() + " - ";
                array<Card2^>^ holeCards = players[i]->getHoleCards();
                if (holeCards[0] != nullptr && holeCards[1] != nullptr) {
                    playerStr += holeCards[0]->toString() + " " + holeCards[1]->toString();
                }

                System::Windows::Forms::Label^ playerLabelc;
                switch (i) {
                case 0: playerLabelc = player_Label; break;
                case 1: playerLabelc = AI1Label; break;
                case 2: playerLabelc = AI2Label; break;
                case 3: playerLabelc = AI3Label; break;
                }
                playerLabelc->Text = playerStr;
                playerLabelc->Refresh();
            }

            // Update pot
            this->potLabel->Text = "Pot: $" + table->getPot();
            potLabel->Refresh();
        }
    };
}