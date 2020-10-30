# OpenMiko Submodules

This project uses Git submodules. More information can be found here:
https://git-scm.com/book/en/v2/Git-Tools-Submodules

Remember to run:

`git submodule update --recursive`

To update your submodules.

The reason for this is allow easy development of both git trees separately:
`v4l2rtspserver` and `ingenic_videocap`.

If you are working with submodules it may be convenient to set the following:

`git config --global status.submoduleSummary true`


## Resources
For further reading:

- https://medium.com/@porteneuve/mastering-git-submodules-34c65e940407



