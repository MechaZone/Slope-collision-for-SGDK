You can follow the progress of my Sega Genesis/Mega Drive game, Gilgamech, from my free Patreon blog
patreon.com/mechazone

This code isn't perfect, but maybe you can improve it or at least learn a thing or two from it?

I originally built the baisc collision code without slope detection based on tutorials by Pigsy. 
https://www.youtube.com/playlist?list=PL1xqkpO_SvY2_rSwHTBIBxXMqmek--GAb

Then I was later able to adapt it to handle slopes with help from SpaceBruce and this tutorial.
https://spacebruce.netlify.app/posts/tilemap/

My main character uses a 48x48 footprint so I had to add more detection points than either tutorial accounted for. I also limited the slopes to just two different angles, but the system can easily accomodate a third by just editing the mapCollision files.
