# VPC notes

## loading order
default.vgc is always loaded first. this sucks and isnt super ideal
but its easy to change.

default.vgc defines game strings you can pass to vpc 'vpc /gamestr' to
pick which conditional gets set.
default.vgc loads groups and projects (all)

gotcha: VPC seems to ignore VPCs not in a group (thus no sln),
even when in a project unless you force generate them..

## adding new junk that you want to generate projects for
1. create your custom vpc
2. (Optional) add the vpc $Project to a group in your group file (see default.vgc)
3. run 'vpc +ProjectName' to make it spit out vcxprojs. if it cant find your
'ProjectName', you may need to add the vpc to your project file in default.vgc.
4. if you want to create a solution for a group (or single project for some wacky reason) pass '/mksln <sln name>' to vpc
