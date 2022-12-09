# congratbot

Congratulation bot for Mastodon, written in C++.

The *congratbot* binary can be used to back a bot account on the Fediverse.
When mentioned, Congratbot replies to the message where it is mentioned with a
congratulatory post according to its configuration.

You can tag it into posts where people are writing about their achievements or
projects, and have them properly congratulated by a third party that is not
just you.

## Config File

The configuration is provided in *text protocol buffer* format, It contains the
following settings:

*instance_url* (string): the name of the instance that contains the account.
For example, *example.org*.

*access_token* (string): Mastodon access token the bot uses. This can be found
under *Development* -> *Applications* in the Mastodon Settings UI of the bot
account.

*congrat_variants*: sub-message containing a list of congratulation strings.

*congrat_variants* -> *congratulation_message* (string): option of text to use
for replies. A Reply will be picked from this list at random.

## Invocation

The command line arguments for the *congratbot* binary are as follows:

 - `-c`: followed by the path to the configuration file.
