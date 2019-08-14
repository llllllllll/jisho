jisho
=====

Tools for interacting with https://jisho.org/.

Usage
-----

The ``jisho`` utility can be used to search for one or more words from the command line.

.. code-block::

   $ ./jisho テスト
   テスト(テスト); jlpt={n5}; common
     Noun, Suru verb
       test
     Wikipedia definition
       Test

   $ ./jisho 漢字
   漢字(かんじ); jlpt={n5}; common
     Noun
       Chinese characters; kanji
     Wikipedia definition
       Chinese characters

   $ ./jisho romaji
   ローマ字(ローマじ); jlpt={n2, n1}; common
     Noun
       Latin alphabet; Roman alphabet
     Noun
       rōmaji; romanized Japanese; system of transliterating Japanese into the Latin alphabet
     Wikipedia definition
       Romanization of Japanese

   $ ./jisho 複数 言葉
   複数(ふくすう); jlpt={n2}; common
     No-adjective, Noun
       plural; multiple; several
     Wikipedia definition
       Plural
   言葉(ことば); jlpt={n5}; common
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

sqlite
~~~~~~

``jisho`` can store words that it looks up in a local sqlite database.
This feature is designed to help build a local database of words searched to be exported to a flashcard program like `anki <https://github.com/dae/anki>`__.

First, a new database needs to be initialized using:

.. code-block::

   $ jisho --sqlite-db-init /path/to/db

This will create a new database with two tables:

- ``words``
- ``senses``

These tables can be joined ``on words.word = senses.word`` to get all of the senses for a given word.

To write a word or words to an initialized sqlite database, use:

.. code-block::

   $ jish --sqlite WORDS TO SEARCH

Words that fail to resolve will be skipped as will words that are already in the database.

By default, words will still be printed to stdout.
To disable writing the definitions to stdout, and only write the words to the sqlite database, the ``--no-stdout`` flag may be passed.

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

To facilitate quickly searching words, ``jisho`` includes a tool called the ``jisho-word-grabber``.
The word grabber is designed to make it easy to look up words and add them to a flashcard deck.

``jish-word-grabber`` searches the currently selected (highlighted) text and displays the definition(s) at the top of the screen.
``jisho-word-grabber`` also adds the selected to a sqlite database stored at ``JISHO_WORD_GRABBER_FILE``, which defaults to ``~/.jisho-word-grabber.sqlite``.

``jisho-word-grabber`` is meant to be bound to a hotkey, for example, ``M-\```.
Using a hotkey makes it minimally intrusive to search for and add new words to a flashcard deck, making it more likely new words will get added.

Currently ``jisho-word-grabber`` only works on GNU+Linux and has the following additional runtime dependencies:

- xclip (retrieving the selected text)
- dmenu (displaying the definitions)
- bash (used to script ``jisho``, xclip, and dmenu)

Building
--------

``jisho`` depends on the following libraries:

- curl (http requests)
- Boost.Program_options (CLI argument parsing)
- Boost.PropertyTree (JSON parsing)
- sqlite (sqlite storage for words)

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
