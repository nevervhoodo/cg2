Машграф. Часть 2. Задание по праку
==================================


Структура работы
====================================

Моей задачей было реализовать подсчет признаков картинок с SSE и без

За формирование вектора признаков отвечает функция myhog(), описанная 
в mycode.h и реализованная в mycode.cpp? которая последовательно выполняет следующее:

* Преобразование цветного изображения в градации серого makegray()

* Вычисление горизонтальных и вертикальных составляющих градиентов яркости
  в каждом пикселе по методу Собеля, а также вычисление модулей этих векторов и 
  аргументов. Есть реализация с SSE и без. sobelfilter()

* Вычисление гистограммы градиентов для картинки counthog(). С использоавнием 
вычисления гистограммы в каждой клетке изображения cellhog() и поледующей 
нормализацией вектора normalization(). Есть реализация с SSE и без.

Затем, в dop.h и dop.cpp описаны и реализованы дополнительные части задания:

* Вычисление пирамиды дескрипторов piramid(). Есть реализация с SSE и без.

* Вычисление цветовых признаков colorfeatures()

* Преобразование вектора признаков для нелинейного ядра SVM unlinearsvmkernel().
Есть реализация с SSE и без.

Что получилось с SSE и без
===========================

Для использования SSE при запуске программы нужно задать флаг --sse 

Время работы всей программы, включая тренировку и работу классификатора

* Time with no code: 120s
* Time with SSE code: 102s
on multiclass

* Time with no code: 40s
* Time with SSE code: 35s
on binary

Сравнение времени работы с/без SSE фильтра Собеля в Google test на картинке Lenna.bmp:
* Time with no code: 0.3s
* Time with SSE code: 0.1s
