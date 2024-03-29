# asm-1d-dp

## Условие задачи

Реализуйте на языке ассемблера armv8 (AArch64) функцию с сигнатурой:
```c
extern uint64_t longest_inc_subseq(int64_t* array, uint64_t* help_array, uint64_t size);
```

Эта функция получает числовой массив `array` длины `size` и возвращает длину наибольшей строго
возрастающей подпоследовательности в нём. Массив `help_array` также имеет длину `size`, но при этом
не содержит заданных значений и нужен лишь в помощь для реализации функции. Задача решается с
помощью алгоритмов динамического программирования.

Замечание: для упрощения задачи массив `help_array` создаётся и передаётся внешним образом. Но это
можно сделать и внутри функции, выделив память на стеке (в случае не слишком больших `size`) или из
кода на ассемблере вызвав `malloc` или системные вызовы `sbrk` и `mmap`, через которые `malloc`
реализован.

## Замечания

В этой задаче, как и в задаче asm-add, часть тестов проверяет отсутствие
ошибок неопределённого поведения при работе с регистрами.

Инструменты для кросс-компиляции и эмуляции те же, что и в задаче asm-add.
