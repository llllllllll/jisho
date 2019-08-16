model_name = 'jisho-sqlite'

field_names = (
    'word',
    'reading',
    'back',
)

css = """\
.card {
 font-family:"BIZ UDPGothic";
 font-size: 25px;
 text-align: center;
 color: White;
 background-color: Black;
}
"""

front_template = """\
<span style="font-size: 50px;  ">{{word}}</span>
"""

back_template = """\
{{FrontSide}}

<hr id=answer>
<span style="font-size: 35px; ">{{reading}}</span>
<br>
<span style="font-size: 30px; ">{{back}}</span>
<br>
"""


def model(col):
    """Look up or create the model needed for jisho-sqlite cards.
    """
    models = col.models
    m = models.byName(model_name)
    if m is not None:
        # the model already exists
        return m

    m = models.new(model_name)
    m['css'] = css

    for ix, name in enumerate(field_names):
        field = models.newField(name)
        field['ord'] = ix
        models.addField(m, field)

    t = models.newTemplate('jisho-sqlite-translate')
    t['qfmt'] = front_template  # question format
    t['afmt'] = back_template   # answer format
    models.addTemplate(m, t)
    models.add(m)
    return m
