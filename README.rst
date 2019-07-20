jisho
=====

Tools for interacting with https://jisho.org/.

Usage
-----

The ``jisho`` utility can be used to search for one or more words from the command line.

.. code-block::

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

CSV
~~~

``jisho`` can also generate CSV files which can be imported into flashcard programs like `anki <https://github.com/dae/anki>`__.
To do this, pass the ``--csv`` flag.

.. code-block::

   $ ./jisho テスト 漢字 複数 --csv
   "テスト"	"テスト"	"Noun, Suru verb: test<br/>Wikipedia definition: Test<br/>"
   "漢字"	"かんじ"	"Noun: Chinese characters; kanji<br/>Wikipedia definition: Chinese characters<br/>"
   "複数"	"ふくすう"	"No-adjective, Noun: plural; multiple; several<br/>Wikipedia definition: Plural<br/>"

By default, the delimiter is a tab character to match anki's preferences, though can be changed, for example:


.. code-block::

   $ ./jisho テスト 漢字 複数 --csv --csv-field-delim='|'
   "テスト"|"テスト"|"Noun, Suru verb: test<br/>Wikipedia definition: Test<br/>"
   "漢字"|"かんじ"|"Noun: Chinese characters; kanji<br/>Wikipedia definition: Chinese characters<br/>"
   "複数"|"ふくすう"|"No-adjective, Noun: plural; multiple; several<br/>Wikipedia definition: Plural<br/>"

Word Grabber
~~~~~~~~~~~~

To facilitate quickly searching words to be added as a flashcard, ``jisho`` includes a tool called the ``jisho-word-grabber``.
The word grabber is designed to make it easy to collect words while reading.

``jisho-word-grabber`` searches the currently selected word and adds it as a new row to a CSV stored at ``JISHO_WORD_GRABBER_FILE``, which defaults to ``~/jisho-word-grabber.csv``.

``jisho-word-grabber`` is meant to be bound to a hotkey, for example, ``M-\```.
Using a hotkey makes it minimally intrusive to add new words to a flashcard deck, making it more likely new words will get added.

Currently ``jisho-word-grabber`` only works on GNU+Linux and requires ``xclip``.

Building
--------

``jisho`` depends on the following libraries:

- curl (http requests)
- Boost.Program_options (CLI argument parsing)
- Boost.PropertyTree (JSON parsing)

``jisho`` is build with ``CMake``.
To build, run the following:

.. code-block:: bash

   $ mkdir build
   $ cd build
   $ cmake ..
   $ make

To do a release build (enable optimizations, strip symbols, etc.):

.. code-block:: bash

   $ cd build
   $ cmake -DCMAKE_BUILD_TYPE=Release ..
   $ make

Future Work
-----------

- automatic `anki <https://github.com/dae/anki>`__ deck integration with ``jish-word-grabber``
- pacman package
- brew package
- windows installer
