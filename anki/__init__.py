from anki import importing

from .importer import JishoSQLiteImporter

importing.Importers = importing.Importers + (
    ("Jiso sqlite database (*.jishodb)", JishoSQLiteImporter),
)
