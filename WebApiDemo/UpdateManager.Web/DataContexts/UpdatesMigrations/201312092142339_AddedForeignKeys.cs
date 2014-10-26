namespace UpdateManager.Web.DataContexts.UpdatesMigrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class AddedForeignKeys : DbMigration
    {
        public override void Up()
        {
            AlterColumn("UpdateManager.PackageFiles", "FileName", c => c.String(maxLength: 255));
            AlterColumn("UpdateManager.PackageFiles", "FilePath", c => c.String(maxLength: 500));
            AlterColumn("UpdateManager.UpdatePackages", "CreatedBy", c => c.String(maxLength: 255));
            AlterColumn("UpdateManager.Updates", "ChangeLogPath", c => c.String(maxLength: 500));
            AlterColumn("UpdateManager.Updates", "CreatedBy", c => c.String(maxLength: 255));
            CreateIndex("UpdateManager.PackageFiles", "UpdatePackageId");
            CreateIndex("UpdateManager.UpdatePackages", "UpdateId");
            AddForeignKey("UpdateManager.PackageFiles", "UpdatePackageId", "UpdateManager.UpdatePackages", "Id", cascadeDelete: true);
            AddForeignKey("UpdateManager.UpdatePackages", "UpdateId", "UpdateManager.Updates", "Id", cascadeDelete: true);
        }
        
        public override void Down()
        {
            DropForeignKey("UpdateManager.UpdatePackages", "UpdateId", "UpdateManager.Updates");
            DropForeignKey("UpdateManager.PackageFiles", "UpdatePackageId", "UpdateManager.UpdatePackages");
            DropIndex("UpdateManager.UpdatePackages", new[] { "UpdateId" });
            DropIndex("UpdateManager.PackageFiles", new[] { "UpdatePackageId" });
            AlterColumn("UpdateManager.Updates", "CreatedBy", c => c.String());
            AlterColumn("UpdateManager.Updates", "ChangeLogPath", c => c.String());
            AlterColumn("UpdateManager.UpdatePackages", "CreatedBy", c => c.String());
            AlterColumn("UpdateManager.PackageFiles", "FilePath", c => c.String());
            AlterColumn("UpdateManager.PackageFiles", "FileName", c => c.String());
        }
    }
}
