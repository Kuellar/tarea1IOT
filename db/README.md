## Setup Database
---

### 1. Install MSQL [Linux](https://dev.mysql.com/doc/mysql-installation-excerpt/8.0/en/linux-installation.html) - [Windows](https://dev.mysql.com/doc/mysql-installation-excerpt/8.0/en/windows-installation.html) - [MacOS](https://dev.mysql.com/doc/mysql-installation-excerpt/8.0/en/macos-installation.html)

> Linux Ubuntu:
>
> `$ sudo apt update`
>
> `$ sudo apt install mysql-server`

> Check installation:
>
> `$ mysql --version`
>
> `$ sudo systemctl status mysql`

### 2. Create root password

> `$ sudo mysql_secure_installation`

> En caso de Error...
>
> `$ sudo mysql`
>
> `mysql> ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password by 'mynewpassword';`

### 3. Create Database

> `$ cd path/to/db`
>
> `$ mysql -u root -p < tables.sql`

