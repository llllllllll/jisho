jisho
=====

CLI for interacting with https://jisho.org/.

Usage
-----

.. code-block:: bash

   $ ./jisho
   usage: ./jisho WORD [WORD...]

   $ ./jisho テスト
   テスト(テスト)
     Noun, Suru verb
       test
     Wikipedia definition
       Test

   $ ./jisho 漢字
   漢字(かんじ)
     Noun
       Chinese characters;kanji
     Wikipedia definition
       Chinese characters

   $ ./jisho romaji
   ローマ字(ローマじ)
     Noun
       Latin alphabet;Roman alphabet
     Noun
       rōmaji;romanized Japanese;system of transliterating Japanese into the Latin alphabet
     Wikipedia definition
       Romanization of Japanese

Future Work
-----------

- bulk search
- csv export
- `anki <https://github.com/dae/anki>`__ plugin
