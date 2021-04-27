# Interpretator-of-Model-Language

Репозиторий содержит код для интерпретации программ на модельном языке. 
- Основной файл: main.cpp;
- Лексический анализатор описан в файле Lex.cpp;
- Синтаксический анализатор описан в файле Syntax.cpp
- Тестовая программа описана в файле Test.txt

Модельный язык похож на Паскаль, допускает использование переменных, структур данных (struct), арифметических операций над числами, конкатенацию строковых переменных и констант, операторы goto, read(), write(), условный оператор, операторы цикла. Подробнее о допустимых операциях можно узнать из таблиц служебных слов в файле Lex.cpp (структуры Scanner::TW и Scanner::TD).
