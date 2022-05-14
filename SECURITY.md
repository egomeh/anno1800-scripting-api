# Security Policy

## Reporting a Vulnerability

## Report a security vulnerability

- For any security vulnerability (regardless of its importance), we strongly encourage you to anonymously and secretly contact the Anno1800-Scripting-API team. The goal is to avoid as many incidents as possible that could be created by this vulnerability. To do this, you must avoid publishing this vulnerability through "issues," or on social networks, etc.

- To report a vulnerability, we simply offer the following model:

```
	- <title> // name well describing the flaw
	- <description> // How is this a flaw, what is the behavior normally expected and the one obtained?
			// Importance of the vulnerability ?
	- <code> // source code of the vulnerability

	[- <operating system> // OS on which the vulnerability is / could be present
	- <locations> // localization of the lines of code in each file that may be sensitive to this vulnerability
	- <examples> // Example uses of the code leading to this vulnerability
	- <contact> // way to contact you privately]
```

All the parts in square brackets are optional but could still be of great help.



## Security vulnerability correction process

1) Once made aware, we will investigate about this, we will check where it can be present in the code and develop a fix as soon as possible, with the utmost discretion.

2) Thereafter, we will encourage contributors and users of our code to update to a this new version
    - at the beginning without specifying that there is a fault
    - then specifying it, without details

3) finally, we will publish a ticket informations on this flaw detailing and correction.

## Monitoring

- However, even after the patching process is complete, we will continue to monitor this flaw, as one patch may not be enough.

- We will regularly check that this flaw is not present in other parts of the source code.

## Limits

- Of course this will not prevent the discovery of the vulnerability and its publication before the end of the process, so it can be shortened.

* for example if the vulnerability is made public before its correction, a patch will be created and published as soon as possible, in parallel we will publish an information ticket detailing it and when the patch is available, we encourage users and contributors to update their source codes to this version. *
