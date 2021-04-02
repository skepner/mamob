# mamob
Magic Mouse Middle Button click handler

# usage
Go to your system preferences -> security -> privacy -> accessibility,
add mamob there (if it is not present yet) and check it.

ln -s $(pwd)/eu.mamob-keep.plist ~/Library/LaunchAgents
launchctl load -w ~/Library/LaunchAgents/eu.mamob-keep.plist
launchctl list | grep mamob

