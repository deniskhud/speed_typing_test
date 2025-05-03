#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include<chrono>
#include<SFML/Graphics.hpp>

using namespace std;

vector<string> loadWordsFromFile(const string& filename) {
    vector<string> words;
    ifstream file(filename);
    string word;
    while (file >> word) {
        words.push_back(word);
    }
    return words;
}

string getRandomWord(const vector<string>& words) {
    static mt19937 rng(random_device{}());
    uniform_int_distribution<size_t> dist(0, words.size() - 1);
    return words[dist(rng)];
}

int main()
{
    
    setlocale(LC_ALL, "ru");
    sf::RenderWindow window(sf::VideoMode(800, 600), "speed typing test");
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);
    if (!window.hasFocus()) {
        window.requestFocus();
    }

    // Создание объекта шрифта
    sf::Font font;
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        cerr << "Не удалось загрузить шрифт!" << endl;
        return -1;
    }

    // Создание текста для отображения слова
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    // Загрузка слов из файла
    vector<string> words = loadWordsFromFile("words.txt");

    // Переменная для текущего слова
    vector<string> currentWords;
    for (int i = 0; i < 5; ++i) {
        currentWords.push_back(getRandomWord(words));
    }
    
    sf::Text wordsText("", font, 30);
    wordsText.setOrigin(wordsText.getLocalBounds().width / 2, wordsText.getLocalBounds().height / 2);
    wordsText.setPosition(window.getSize().x / 2, window.getSize().y - 500);
    wordsText.setFillColor(sf::Color::White);

    string typedWord = ""; // Храним введенное слово
    sf::Text typedText("", font, 30);
    typedText.setPosition(window.getSize().x / 2, window.getSize().y - 450);
    typedText.setFillColor(sf::Color::Green);

    //information 
    int words_per_min = 0;  //count words per minute

    sf::Text wordsPerMin("", font, 30);
    wordsPerMin.setPosition(50, 300);
    wordsPerMin.setFillColor(sf::Color::Cyan);


    int chars_per_min = 0;  //count chars per minute 

    int correctChars = 0;             // <--- Количество правильно введённых символов
    int totalTypedChars = 0;

    sf::Text charsPerMin("", font, 30);
    charsPerMin.setPosition(50, 400);
    charsPerMin.setFillColor(sf::Color::Cyan);


    double Accuracy = 0;

    sf::Text accuracy("", font, 30);
    accuracy.setPosition(50, 500);
    accuracy.setFillColor(sf::Color::Cyan);
    
    //time
    sf::Clock gameClock;
    sf::Time totalTime = sf::seconds(60);   //timer
    sf::Text timerText("", font, 24);
    timerText.setPosition(50, 20);  //pos
    timerText.setFillColor(sf::Color::Red); //color



    bool isTimeUp = false; // Флаг: закончилось ли время

    sf::Text resultText("Time's up! Final results:", font, 40); // Заголовок результата
    resultText.setFillColor(sf::Color::Yellow);
    resultText.setPosition(50, 200);

    // Главный игровой цикл
    while (window.isOpen())
    {
        
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            // Проверка на нажатие клавиш
            if (event.type == sf::Event::TextEntered) {
                if (isTimeUp) continue;
                // Если нажата клавиша
                if (event.text.unicode == 8) { // Backspace
                    if (!typedWord.empty()) {
                        typedWord.pop_back();
                    }
                }
                //когда я нажимаю пробел, должна пройти проверка на аккуратность введенного слова 
                //когда нажимаем пробел, кидает на новое слово
                else if (event.text.unicode == 32) {  //space
                    if (!typedWord.empty()) {
                        totalTypedChars += typedWord.length();

                        // Подсчёт правильных символов по текущему слову
                        const string& targetWord = currentWords[0];
                        size_t correct = 0;
                        for (size_t i = 0; i < min(typedWord.length(), targetWord.length()); ++i) {
                            if (typedWord[i] == targetWord[i])
                                ++correct;
                        }
                        correctChars += correct;  // <--- Увеличиваем число правильно введённых символов

                        if (typedWord == currentWords[0]) {
                            //++ words
                            words_per_min++;
                            chars_per_min += currentWords[0].length();
                        }
                 
                        //Accuracy = (static_cast<double>(typedWord.length() / currentWords[0].length()) * 100.f);
                        currentWords.push_back(getRandomWord(words));
                        currentWords.erase(currentWords.begin());
                            

                        if (totalTypedChars > 0)
                            Accuracy = static_cast<double>(correctChars) / totalTypedChars * 100.0;
                        
                    }
                    
               
                    typedWord.clear();
                }
                else if (event.text.unicode < 128) { // Если обычный символ
                    typedWord += static_cast<char>(event.text.unicode);
                }
                
                //когда введенное слово правильное, тогда обновляется счетчик букв в минуту и слов в минуту !!!!!!
                
            }
            // Обновление текста
            string displayLine;
            for (const auto& w : currentWords)
                displayLine += w + " ";
            wordsText.setString(displayLine);
            typedText.setString(typedWord);
           

            //обновление счетчика
            wordsPerMin.setString("words per min: " + to_string(words_per_min));
            charsPerMin.setString("chars per min: " + to_string(chars_per_min));
            accuracy.setString("Accuracy: " + to_string(Accuracy));
            
        }
        // Обновление таймера
        sf::Time elapsed = gameClock.getElapsedTime();
        sf::Time remaining = totalTime - elapsed;

        if (remaining <= sf::Time::Zero) {
            timerText.setString("Time: 0");
            isTimeUp = true; 
              
        }
        else {
            int secondsLeft = static_cast<int>(remaining.asSeconds());
            timerText.setString("Time: " + std::to_string(secondsLeft));
        }
      
        
        window.draw(timerText);
        window.draw(wordsText);
        window.draw(typedText);
        window.draw(wordsPerMin);
        window.draw(charsPerMin);
        window.draw(accuracy);
        
        if (isTimeUp) {
            window.draw(resultText); // Рисуем заголовок результатов
        }
        window.display();

        // Очищаем окно
        window.clear();
        
    }

    return 0;
}