namespace UpdateManager.Web.DataContexts.UpdatesMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddedModelValidation : DbMigration
    {
        public override void Up()
        {
            AlterColumn("UpdateManager.PackageFiles", "FileName", c => c.String(nullable: false, maxLength: 255));
            AlterColumn("UpdateManager.PackageFiles", "FilePath", c => c.String(nullable: false, maxLength: 500));
            AlterColumn("UpdateManager.UpdatePackages", "CreatedBy", c => c.String(nullable: false, maxLength: 255));
            AlterColumn("UpdateManager.Updates", "Name", c => c.String(nullable: false, maxLength: 200));
            AlterColumn("UpdateManager.Updates", "Description", c => c.String(nullable: false, maxLength: 500));
            AlterColumn("UpdateManager.Updates", "CreatedBy", c => c.String(nullable: false, maxLength: 255));
        }
        
        public override void Down()
        {
            AlterColumn("UpdateManager.Updates", "CreatedBy", c => c.String(maxLength: 255));
            AlterColumn("UpdateManager.Updates", "Description", c => c.String(maxLength: 500));
            AlterColumn("UpdateManager.Updates", "Name", c => c.String(maxLength: 200));
            AlterColumn("UpdateManager.UpdatePackages", "CreatedBy", c => c.String(maxLength: 255));
            AlterColumn("UpdateManager.PackageFiles", "FilePath", c => c.String(maxLength: 500));
            AlterColumn("UpdateManager.PackageFiles", "FileName", c => c.String(maxLength: 255));
        }
    }
}
