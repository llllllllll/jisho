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
       Chinese characters; kanji
     Wikipedia definition
       Chinese characters

   $ ./jisho romaji
   ローマ字(ローマじ)
     Noun
       Latin alphabet; Roman alphabet
     Noun
       rōmaji; romanized Japanese; system of transliterating Japanese into the Latin alphabet
     Wikipedia definition
       Romanization of Japanese

   $ ./jisho 複数 言葉
   複数(ふくすう)
     No-adjective, Noun
       plural; multiple; several
     Wikipedia definition
       Plural

   言葉(ことば)
     Noun
       language; dialect
     Noun
       word; words; phrase; term; expression; remark
     Noun
       speech; (manner of) speaking
     Noun
       learning to speak; language acquisition
     Wikipedia definition
       Ci (poetry)


Future Work
-----------

- bulk search
- csv export
- `anki <https://github.com/dae/anki>`__ plugin
