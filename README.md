# User guide (Tested for Aalto Virtual Desktop)

## Requirements
- the basics, C++ compiler etc.
- Cmake
- QT 5 or 6
Aalto Virtual Desktop had all of these already

## Getting the code
- clone/copy zip from Gitlab and extract
- open the directory in terminal and change to subdirectory gui

## Create executable
* cmake .
 - to configure
* make
 - to create executable
* ./network_simulator
 - to run the executable

 ## Setting up the network
 - on the bar in top left, click file and then load
 - menu opens up, double-click testnodes.txt
 - the same menu opens up, this time double-click testlinks.txt
 - arrange the nodes to your liking

## Using the program
* right-click nodes to open a menu for actions, or
* click "create packet", the following values will be asked:
 - KNOWN ISSUE: if you close one of the opening windows, the program crashes
 - sending client's name (choose from the existing ones, green squares are clients)
 - receiving server's name (choose from the existing ones, blue things are servers)
 - content for message (cosmetic, but for immersion reasons choose something appropriate (e.g. for Netflix some movie etc))
 - choose a flag of the options presented (what do they mean? see doc/classes.pdf)
* if you didn't choose the cheat flag, right-click the selected client and click send


More documentation in doc/


# Network simulator

This is the template for the projects. Please copy the project description here. 
You can use Markdown language to render it as formatted **HTML** file.

# Group
- Rasmus Salmela
- Niklas Anttila
- Santeri Salmela
- Hamid Aebadi

# Repository organization
Your project implementation should follow the skelaton organization in this repository.
See readme.md files in each folder.

# Project Implementation 
You must use git repository for the work on the project, making frequent enough commits so 
that the project group (and course staff) can follow the progress.

The completed project work will be demonstrated to the group's advisor at a demo session. 
The final demonstrations are arranged on week 50. After the final demonstrations project group 
evaluates another project, and self-evaluates own project. In addition, project members will 
give a confidential individual assessment of each group member

The course staff should be able to easily compile the project work using makefile and related 
instructions provided in the git repository. The final output should be in the **master branch** of the git repository.

# Working practices
Each project group is assigned an advisor from the project teaching personnel. 
There will be a dedicated Teams channel for each project topic to facilitate discussion between 
the groups in the same topic and the advisor. 

**The group should meet weekly.** The weekly meeting does not need to be long if there are no special issues 
to discuss, and can be taken remotely as voice/video chat on the group Teams channel (or Zoom or other similar tool), 
preferably at a regular weekly time. In the meeting the group updates:

- What each member has done during the week
- Are there challenges or problems? Discuss the possible solutions
- Plan for the next week for everyone
- Deviations and changes to the project plan, if any
- After the meetings, the meeting notes will be committed to the project repository in the `Meeting-notes.md` file. 
    * The commits within the week should have some commit messages referring to the meeting notes so 
      that the project advisor can follow the progress.  
    * **The meeting notes should be in English.**

> Everyone may not be able to participate to all meetings, but at least a couple of members should be present in each meeting. 
> Regular absence from meetings will affect in individual evaluation.

# Source code documentation
It is strongly recommended to use Doxygen to document your source code.
Please go over the *Project Guidelines* for details.

# TODOs (Date)
You can create a list of TODOs in this file.
The recommended format is:
- Complete class implementation **foo**. Assigned to \<Member 1\>
- Test ...

Rasmus
- moving the pointers across queues may not work
- maybe normalize all sizes to kilobits/bytes
- make sure that routing table works