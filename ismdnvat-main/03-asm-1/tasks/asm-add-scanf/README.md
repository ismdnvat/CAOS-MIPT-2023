# asm-add-scanf

## Условие задачи

Реализуйте на языке ассемблера x86-64 функцию с сигнатурой:
```c
extern int64_t add_scanf();
```
Эта функция читает со стандартного потока ввода два числа типа long long int, складывает их,
и возвращает результат. Гарантируется, что тип long long int занимает 64 бита. Для чтения чисел
используйте функцию scanf стандартной библиотеки Си.

При реализации пользуйтесь `calling conventions`.

## Замечания

При решении задачи может возникнуть абсолютно неожиданный и непонятно откуда взявшийся segmentation
fault. Если это произойдёт, то подумайте о том, каким должно быть выравнивание стека перед вызовом
внешних функций и как изменяется размер стека в программе, в том числе, как изменяют его функции
push и call. Конечно, segmentation fault может происходить из-за разных причин, но эта здесь
наиболее вероятна. Если segmentation fault не произойдёт и при этом хочется лучше разобраться
в теме, можно подумать о том же и воспроизвести ситуацию, когда он всё-таки случится.

Поясним, для чего нужны два закрытых теста: `Сleaning registers test` и `Saving registers test`.
Они предназначены для детерминированности проверки решения, в частности, если в каком-то регистре
случайно оказалось подходящее значение, но это ничем не гарантируется, тесты должны это поймать,
если был изменён регистр, который функция обязана сохранять, тесты должны это поймать.
