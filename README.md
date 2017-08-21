Vocalist eurorack speech synthesizer

Modes:
- play phrase
plays entire phrase while gate is high.  Pitch and other variables are set at the start and held constant.
- play word
when trigger happens, the next word in the patch is played.  pitch and CC's are determined per at start of word.  Reset button or trigger makes the next word the first word of phrase.
- play syllable
Play one syllable per trigger
- scan phonemes
While gate is high, we scan phonemes.  Pitch is determined constantly, and phoneme position in patch is chosen by (other CV input?)

Word banks:
- N banks of Y words in alphabetical order
- show words on 4-char red LED screen

CV control of bank and word, or at least word
one or more banks are "user"
bank could be next/prev buttons
pick word, "append" appends it to current patch
hold append 2sec to drop last word
need a way to save it into user patch
^ this currently has an issue with mental model - are we playing the selected word, or the patch?

"Stutter" - likelyhood we repeat a phoneme
"Chaos" - (carefully) fuck with internal buffers

Hardware:
4-digit hex LED - maybe 3 digits is enough
maybe LTP-587HR
driven by shift register + address line picker

