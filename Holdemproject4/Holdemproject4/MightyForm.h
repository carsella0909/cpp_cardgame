#pragma once

#include <string>
#include <random>
#include <algorithm>

namespace CardGameApp {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;
    using namespace System::Collections::Generic;

    public ref class Card {
    public:
        String^ suit;
        int number;

        Card(String^ s, int n) : suit(s), number(n) {}

        virtual String^ ToString() override {
            return suit + " " + number;
        }
    };

    public ref class Player {
    public:
        String^ name;
        List<Card^>^ hand;
        int score;

        Player(String^ n) : name(n), score(0) {
            hand = gcnew List<Card^>();
        }

        void DrawCard(Card^ card) {
            hand->Add(card);
        }

        void AddScore(int points) {
            score += points;
        }

        int GetScore() {
            return score;
        }
    };

    public ref class MightyForm : public System::Windows::Forms::Form {
    public:
        MightyForm(void) {
            InitializeComponent();
            InitializeGame();
        }

    protected:
        ~MightyForm() {
            if (components) {
                delete components;
            }
        }

    private:
        System::ComponentModel::Container^ components;
        System::Windows::Forms::Button^ nextPhaseButton;
        System::Windows::Forms::TextBox^ gameOutput;

        List<Player^>^ players;
        List<Card^>^ deck;
        Player^ currentPlayer;
        int currentPhase;
        int contractNumber;
        int rulingScore = 0;
        int oppositionScore = 0;
        String^ contractSuit;
        Player^ friendPlayer;
        Label^ gameStateLabel;
        bool noGiruda;
        bool noFriend;

        void InitializeComponent(void) {
            this->nextPhaseButton = (gcnew System::Windows::Forms::Button());
            this->gameOutput = (gcnew System::Windows::Forms::TextBox());
            this->gameStateLabel = gcnew Label();

            this->nextPhaseButton->Location = System::Drawing::Point(450, 20);
            this->nextPhaseButton->Name = L"nextPhaseButton";
            this->nextPhaseButton->Size = System::Drawing::Size(200, 40);
            this->nextPhaseButton->Text = L"Next Phase";
            this->nextPhaseButton->Click += gcnew System::EventHandler(this, &MightyForm::OnNextPhaseClicked);

            this->gameStateLabel->Location = System::Drawing::Point(50, 50);
            this->gameStateLabel->Name = L"gameStateLabel";
            this->gameStateLabel->Size = System::Drawing::Size(700, 400);
            this->gameStateLabel->Text = L"Game State";

            this->gameOutput->Location = System::Drawing::Point(20, 80);
            this->gameOutput->Multiline = true;
            this->gameOutput->Name = L"gameOutput";
            this->gameOutput->Size = System::Drawing::Size(500, 400);
            this->gameOutput->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;

            // 
            // IndianPokerForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(800, 600);
            this->Controls->Add(this->nextPhaseButton);
            this->Controls->Add(this->gameOutput);
            this->Controls->Add(this->gameStateLabel);
            this->Name = L"MightyForm";
            this->Text = L"마이티";
            this->ResumeLayout(false);
            this->PerformLayout();
        }

        void InitializeGame() {
            players = gcnew List<Player^>();
            deck = gcnew List<Card^>();

            // 카드 덱 초기화
            for each (String ^ suit in gcnew cli::array<String^> { "Spades", "Diamonds", "Hearts", "Clubs" }) {
                for (int number = 1; number <= 13; ++number) {
                    deck->Add(gcnew Card(suit, number));
                }
            }
            deck->Add(gcnew Card("Joker", 0));

            // 카드 덱 섞기
            ShuffleDeck();

            // 플레이어 추가
            players->Add(gcnew Player("User"));
            for (int i = 1; i <= 4; ++i) {
                players->Add(gcnew Player("AI " + i));
            }

            currentPhase = 1;
            DisplayMessage("게임을 시작합니다!");
        }

        void ShuffleDeck() {
            // 매니지드 리스트 deck을 섞기 위해 배열로 변환 후 다시 리스트로 복원
            array<Card^>^ deckArray = deck->ToArray();
            std::random_device rd;
            std::mt19937 g(rd());

            // 배열을 무작위로 섞고 다시 리스트로 변환
            for (int i = deckArray->Length - 1; i > 0; i--) {
                int j = g() % (i + 1);
                Card^ temp = deckArray[i];
                deckArray[i] = deckArray[j];
                deckArray[j] = temp;
            }

            deck->Clear();
            for each (Card ^ card in deckArray) {
                deck->Add(card);
            }
        }

        void DistributeCards() {
            for each (Player ^ player in players) {
                for (int j = 0; j < 10; ++j) {
                    player->DrawCard(deck[deck->Count - 1]);
                    deck->RemoveAt(deck->Count - 1);
                }
            }
            DisplayMessage("카드가 분배되었습니다.");
        }

        void OnNextPhaseClicked(System::Object^ sender, System::EventArgs^ e) {
            switch (currentPhase) {
            case 1:
                StartPhase1();
                break;
            case 2:
                StartPhase2();
                break;
            case 3:
                StartPhase3();
                break;
            default:
                DisplayMessage("게임 종료");
                break;
            }
        }

        void StartPhase1() {
            DistributeCards();
            DetermineContract();
            SetFriend();
            currentPhase = 2;
            DisplayMessage("주공과 프렌드가 설정되었습니다. 게임을 시작합니다.");
        }

        void DetermineContract() {
            currentPlayer = players[0];
            contractNumber = 13;
            contractSuit = "Spades";
            DisplayMessage("공약: " + contractNumber + ", 기루다: " + contractSuit);
        }

        void SetFriend() {
            friendPlayer = nullptr; // 프렌드 임의 설정
            DisplayMessage("프렌드 설정: 첫 라운드 승자가 프렌드가 됩니다.");
        }

        void StartPhase2() {
            for (int round = 1; round <= 10; ++round) {
                PlayRound(round);
            }
            currentPhase = 3;
            DisplayMessage("라운드가 완료되었습니다. 점수 계산을 진행합니다.");
        }

        void PlayRound(int round) {
            DisplayMessage("라운드 " + round + " 시작");
            Card^ highestCard = nullptr;
            Player^ roundWinner = nullptr;

            for each (Player ^ player in players) {
                Card^ cardPlayed = player->hand[player->hand->Count - 1];
                player->hand->RemoveAt(player->hand->Count - 1);
                DisplayMessage(player->name + "가 낸 카드: " + cardPlayed->ToString());

                if (highestCard == nullptr || CompareCards(cardPlayed, highestCard)) {
                    highestCard = cardPlayed;
                    roundWinner = player;
                }
            }

            DisplayMessage("라운드 승자: " + roundWinner->name);
        }

        bool CompareCards(Card^ card1, Card^ card2) {
            if (card1->suit == contractSuit && card2->suit != contractSuit) {
                return true;
            }
            if (card1->suit != contractSuit && card2->suit == contractSuit) {
                return false;
            }
            return card1->number > card2->number;
        }

        void StartPhase3() {
            CalculateScores();

            DisplayMessage("점수를 계산합니다.");
            DisplayMessage("여당 점수: " + rulingScore + ", 야당 점수: " + oppositionScore);

            if (rulingScore >= contractNumber) {
                DisplayMessage("여당 승리!");
                AssignScoresToRulingParty();
            }
            else {
                DisplayMessage("야당 승리!");
                AssignScoresToOpposition();
            }
            currentPhase = 1;
        }

        void CalculateScores() {
            rulingScore = 0;
            oppositionScore = 0;

            for each (Player ^ player in players) {
                for each (Card ^ card in player->hand) {
                    int cardScore = GetCardScore(card);

                    // 주공(여당) 팀 점수와 야당 팀 점수 누적
                    if (player == currentPlayer || player == friendPlayer) {
                        rulingScore += cardScore;
                    }
                    else {
                        oppositionScore += cardScore;
                    }
                }
            }
            this->rulingScore = rulingScore;
            this->oppositionScore = oppositionScore;
        }

        int GetCardScore(Card^ card) {
            if (card->suit == "Joker") {
                return 0;  // Joker는 점수에 포함되지 않음
            }

            // A, K, Q, J, 10 카드만 1점으로 계산
            if (card->number == 1 || card->number == 13 || card->number == 12 || card->number == 11 || card->number == 10) {
                return 1;
            }
            return 0;
        }


        void AssignScoresToRulingParty() {
			int scoreToAdd = rulingScore + contractNumber - 26;
            currentPlayer->AddScore(2*scoreToAdd);
			friendPlayer->AddScore(scoreToAdd);
            for each (Player ^ player in players) {
                if ((player != currentPlayer) && (player != friendPlayer)) {
                    player->AddScore(-scoreToAdd);
                }
            }
        }

        void AssignScoresToOpposition() {
			int scoreToAdd = contractNumber - rulingScore;
            currentPlayer->AddScore(-2 * scoreToAdd);
            friendPlayer->AddScore(-scoreToAdd);
            for each (Player ^ player in players) {
                if ((player != currentPlayer)&&(player != friendPlayer)) {
                    player->AddScore(scoreToAdd);
                }
            }
        }

        void DisplayMessage(String^ message) {
            gameStateLabel->Text = message;
            gameStateLabel->Refresh();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    };
}
