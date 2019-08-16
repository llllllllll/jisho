import sqlite3
from textwrap import dedent

from anki.importing.noteimp import NoteImporter, ForeignNote

from .model import model


def groupby(field, rows):
    out = {}
    for row in rows:
        out.setdefault(row[field], []).append(row)
    return out


class JishoSQLiteImporter(NoteImporter):
    needMapper = False

    # don't allow duplicate words, we should group all the senses into a single
    # card
    importMode = 0

    def __init__(self, *args):
        super().__init__(*args)
        self._rows = None
        self.model = model(self.col)
        self.mapping = [
            'word',
            'reading',
            'back',
        ]

    def foreignNotes(self):
        """Parse notes from the rows read in ``open``.
        """
        self.open()

        out = []
        for word, senses in groupby(0, self._rows).items():
            back = []
            usually_written_using_kana_alone = False
            for row in senses:
                reading = row[1]
                if row[-1]:
                    usually_written_using_kana_alone = True
                back.append(f'{row[3]}: {row[2]}')
            back = '\n'.join(back)

            note = ForeignNote()
            note.fields = [
                reading if usually_written_using_kana_alone else word,
                reading,
                back,
            ]
            out.append(note)

        return out

    def open(self):
        """Parse the top line and determine the pattern and number of fields.
        """
        rows = self._rows
        if rows is None:
            with sqlite3.connect(self.file) as conn:
                curr = conn.cursor()
                try:
                    rows = self._rows = curr.execute(dedent(
                        """\
                        select
                            words.word,
                            words.reading,
                            senses.def,
                            senses.pos,
                            senses.usually_written_using_kana_alone
                        from
                            words
                        join
                            senses
                        on
                            words.word == senses.word
                        """,
                    )).fetchall()
                except Exception as e:
                    self.log.append(f'query failed: {e}')

        return rows

    def fields(self):
        """The number of fields.
        """
        return 3
